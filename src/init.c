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
 * (Not all of the) Initialisation routines for FreeDroid.
 * ---------------------------------------------------------------------- */

/*
 * This file has been checked for remains of german comments.  If you still
 * find any, please kill them mercilessly :)
 */

#define _init_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"
#include "text.h"
#include "ship.h"


void Init_Game_Data( char* Datafilename );
void Get_Bullet_Data ( char* DataPointer );
char* DebriefingText;
char* DebriefingSong;
char* NextMissionName;
char Previous_Mission_Name[1000];

/* ---------------------------------------------------------------------- 
 * This function displays a startup status bar that shows a certain
 * percentage of loading done.
 * ---------------------------------------------------------------------- */
void 
ShowStartupPercentage ( int Percentage )
{
  SDL_Rect Bar_Rect;
  Uint32 FillColor = SDL_MapRGB( Screen->format, 0 , 255 , 0 ) ; 

  Bar_Rect . x = 200 ;
  Bar_Rect . y = 200 ;
  Bar_Rect . w = 2 * Percentage ;
  Bar_Rect . h = 30 ;

  SDL_FillRect ( Screen , & Bar_Rect , FillColor ) ;

  PrintString ( Screen , 200 + 80 , 200 + 4 , "%d%%", Percentage ) ;

  SDL_UpdateRect ( Screen , 200 , 200 , 200 , 30  ) ;

}; // void ShowStartupPercentage ( int Percentage )

/* ----------------------------------------------------------------------
 * This function loads all the constant variables of the game from
 * a data file, that should be optimally human readable.
 * ---------------------------------------------------------------------- */
void
Get_General_Game_Constants ( void* DataPointer )
{
  char *ConstantPointer;
  char *EndOfDataPointer;

#define CONSTANTS_SECTION_BEGIN_STRING "*** Start of General Game Constants Section: ***"
#define CONSTANTS_SECTION_END_STRING "*** End of General Game Constants Section: ***"
#define COLLISION_LOSE_ENERGY_CALIBRATOR_STRING "Energy-Loss-factor for Collisions of Influ with hostile robots="
#define BLAST_RADIUS_SPECIFICATION_STRING "Radius of explosions (as far as damage is concerned) in multiples of tiles="
#define DRUID_RADIUS_X_SPECIFICATION_STRING "'Radius' of droids in x direction="
#define DRUID_RADIUS_Y_SPECIFICATION_STRING "'Radius' of droids in x direction="
#define BLAST_DAMAGE_SPECIFICATION_STRING "Amount of damage done by contact to a blast per second of time="
#define TIME_FOR_DOOR_MOVEMENT_SPECIFICATION_STRING "Time for the doors to move by one subphase of their movement="


  ConstantPointer = LocateStringInData ( DataPointer , CONSTANTS_SECTION_BEGIN_STRING );
  EndOfDataPointer = LocateStringInData ( DataPointer , CONSTANTS_SECTION_END_STRING );

  DebugPrintf ( 2 , "\n\nStarting to read contents of General Game Constants section\n\n");

  // Now we read in the speed calibration factor for all bullets
  ReadValueFromString( DataPointer , COLLISION_LOSE_ENERGY_CALIBRATOR_STRING , "%lf" , 
		       &collision_lose_energy_calibrator , EndOfDataPointer );

  // Now we read in the blast radius
  ReadValueFromString( DataPointer , BLAST_RADIUS_SPECIFICATION_STRING , "%lf" , 
		       &Blast_Radius , EndOfDataPointer );

  // Now we read in the druid 'radius' in x direction
  ReadValueFromString( DataPointer , DRUID_RADIUS_X_SPECIFICATION_STRING , "%lf" , 
		       &Druid_Radius_X , EndOfDataPointer );
  ReadValueFromString( DataPointer , DRUID_RADIUS_Y_SPECIFICATION_STRING , "%lf" , 
		       &Druid_Radius_Y , EndOfDataPointer );

  // Now we read in the blast damage amount per 'second' of contact with the blast
  ReadValueFromString( DataPointer ,  BLAST_DAMAGE_SPECIFICATION_STRING , "%lf" , 
		       &Blast_Damage_Per_Second , EndOfDataPointer );

  // Now we read in the speed of the conveyor belt
  ReadValueFromString( DataPointer ,  "Conveyor belt speed (in squares per second )=", "%lf" , 
		       &Conveyor_Belt_Speed , EndOfDataPointer );

  // Now we read in the time is takes for the door to move one phase 
  ReadValueFromString( DataPointer ,  TIME_FOR_DOOR_MOVEMENT_SPECIFICATION_STRING , "%lf" , 
		       &Time_For_Each_Phase_Of_Door_Movement , EndOfDataPointer );

  DebugPrintf( 1 , "\nvoid Get_General_Game_Constants ( void* DataPointer ): end of function reached." );

} // void Get_General_Game_Constants ( void* DataPointer )

/*----------------------------------------------------------------------
 * This function reads in all the bullet data from the freedroid.ruleset file,
 * but IT DOES NOT LOAD THE FILE, IT ASSUMES IT IS ALREADY LOADED and
 * it only receives a pointer to the start of the bullet section from
 * the calling function.
 ----------------------------------------------------------------------*/
void 
Get_Bullet_Data ( char* DataPointer )
{
  char *BulletPointer;
  char *EndOfBulletData;
  int i;
  int BulletIndex=0;

  double bullet_speed_calibrator;
  double bullet_damage_calibrator;

#define BULLET_SECTION_BEGIN_STRING "*** Start of Bullet Data Section: ***" 
#define BULLET_SECTION_END_STRING "*** End of Bullet Data Section: ***" 
#define NEW_BULLET_TYPE_BEGIN_STRING "** Start of new bullet specification subsection **"

#define BULLET_RECHARGE_TIME_BEGIN_STRING "Time is takes to recharge this bullet/weapon in seconds :"
#define BULLET_SPEED_BEGIN_STRING "Flying speed of this bullet type :"
#define BULLET_DAMAGE_BEGIN_STRING "Damage cause by a hit of this bullet type :"
  // #define BULLET_NUMBER_OF_PHASES_BEGIN_STRING "Number of different phases that were designed for this bullet type :"
#define BULLET_ONE_SHOT_ONLY_AT_A_TIME "Cannot fire until previous bullet has been deleted : "
#define BULLET_BLAST_TYPE_CAUSED_BEGIN_STRING "Type of blast this bullet causes when crashing e.g. against a wall :"

#define BULLET_SPEED_CALIBRATOR_STRING "Common factor for all bullet's speed values: "
#define BULLET_DAMAGE_CALIBRATOR_STRING "Common factor for all bullet's damage values: "

  BulletPointer = LocateStringInData ( DataPointer , BULLET_SECTION_BEGIN_STRING );
  EndOfBulletData = LocateStringInData ( DataPointer , BULLET_SECTION_END_STRING );

  DebugPrintf ( 1 , "\n\nStarting to read bullet data...\n\n");
  //--------------------
  // At first, we must allocate memory for the droid specifications.
  // How much?  That depends on the number of droids defined in freedroid.ruleset.
  // So we have to count those first.  ok.  lets do it.

  Number_Of_Bullet_Types = CountStringOccurences ( DataPointer , NEW_BULLET_TYPE_BEGIN_STRING ) ;

  // Not that we know how many bullets are defined in freedroid.ruleset, we can allocate
  // a fitting amount of memory, but of course only if the memory hasn't been allocated
  // aready!!!
  //
  // If we would do that in any case, every Init_Game_Data call would destroy the loaded
  // image files AND MOST LIKELY CAUSE A SEGFAULT!!!
  //
  if ( Bulletmap == NULL )
    {
      i=sizeof(bulletspec);
      Bulletmap = MyMalloc ( i * ( Number_Of_Bullet_Types + 1 ) + 1 );
      DebugPrintf ( 1 , "\nvoid Get_Bullet_Data( char* DatapPointer ) : We have counted %d different bullet types in the game data file." , Number_Of_Bullet_Types );
      // DebugPrintf ( 0 , "\nMEMORY HAS BEEN ALLOCATED.\nTHE READING CAN BEGIN.\n" );
      // getchar();
    }

  //--------------------
  // Now we start to read the values for each bullet type:
  // 
  BulletPointer=DataPointer;

  while ( (BulletPointer = strstr ( BulletPointer, NEW_BULLET_TYPE_BEGIN_STRING )) != NULL)
    {
      DebugPrintf (1, "\n\nFound another Bullet specification entry!  Lets add that to the others!");
      BulletPointer ++; // to avoid doubly taking this entry

      BulletIndex++;
    }

  //--------------------
  // Now that the detailed values for the bullets have been read in,
  // we now read in the general calibration contants and after that
  // the start to apply them right now, so they also take effect.
  
  DebugPrintf (1, "\n\nStarting to read bullet calibration section\n\n");

  // Now we read in the speed calibration factor for all bullets
  ReadValueFromString( DataPointer ,  BULLET_SPEED_CALIBRATOR_STRING , "%lf" , 
		       &bullet_speed_calibrator , EndOfBulletData );

  // Now we read in the damage calibration factor for all bullets
  ReadValueFromString( DataPointer ,  BULLET_DAMAGE_CALIBRATOR_STRING , "%lf" , 
		       &bullet_damage_calibrator , EndOfBulletData );

  //--------------------
  // Now that all the calibrations factors have been read in, we can start to
  // apply them to all the bullet types
  //
  for ( i = 0 ; i < Number_Of_Bullet_Types ; i++ )
    {
      // Bulletmap[i].speed *= bullet_speed_calibrator;
      // Bulletmap[i].damage *= bullet_damage_calibrator;
    }

  DebugPrintf (1, "\nEnd of Get_Bullet_Data ( char* DataPointer ) reached.");
} // void Get_Bullet_Data ( char* DataPointer );

/*----------------------------------------------------------------------
 * This function reads in all the item data from the freedroid.ruleset file,
 * but IT DOES NOT LOAD THE FILE, IT ASSUMES IT IS ALREADY LOADED and
 * it only receives a pointer to the start of the bullet section from
 * the calling function.
 ----------------------------------------------------------------------*/
void 
Get_Item_Data ( char* DataPointer )
{
  char *ItemPointer;
  char *EndOfItemData;
  // int i;
  int ItemIndex=0;
  char *YesNoString;

  // double bullet_speed_calibrator;
  // double bullet_damage_calibrator;

#define ITEM_SECTION_BEGIN_STRING "*** Start of item data section: ***"
#define ITEM_SECTION_END_STRING "*** End of item data section: ***"
#define NEW_ITEM_TYPE_BEGIN_STRING "** Start of new item specification subsection **"

#define ITEM_NAME_INDICATION_STRING "Item name=\""
#define ITEM_DESCRIPTION_INDICATION_STRING "Item description text=\""
#define ITEM_CAN_BE_APPLIED_IN_COMBAT "Item can be applied in combat=\""
#define ITEM_CAN_BE_INSTALLED_IN_INFLU "Item can be installed in influ=\""
#define ITEM_CAN_BE_INSTALLED_IN_WEAPON_SLOT "Item can be installed in weapon slot=\""
#define ITEM_CAN_BE_INSTALLED_IN_DRIVE_SLOT "Item can be installed in drive slot=\""
#define ITEM_CAN_BE_INSTALLED_IN_ARMOUR_SLOT "Item can be installed in armour slot=\""
#define ITEM_CAN_BE_INSTALLED_IN_SHIELD_SLOT "Item can be installed in shield slot=\""
#define ITEM_CAN_BE_INSTALLED_IN_SPECIAL_SLOT "Item can be installed in special slot=\""
#define ITEM_CAN_BE_INSTALLED_IN_AUX_SLOT "Item can be installed in aux slot=\""

#define ITEM_CAN_BE_INSTALLED_IN_SLOT_WITH_NAME "Item can be installed in slot with name=\""
#define ITEM_CAN_BE_BOUGHT_IN_SHOP "Item can be bought in shop=\""

#define ITEM_GUN_IGNORE_WALL "Item as gun: ignore collisions with wall=\""

#define ITEM_RECHARGE_TIME_BEGIN_STRING "Time is takes to recharge this bullet/weapon in seconds :"
#define ITEM_SPEED_BEGIN_STRING "Flying speed of this bullet type :"
#define ITEM_DAMAGE_BEGIN_STRING "Damage cause by a hit of this bullet type :"
  // #define ITEM_NUMBER_OF_PHASES_BEGIN_STRING "Number of different phases that were designed for this bullet type :"
#define ITEM_ONE_SHOT_ONLY_AT_A_TIME "Cannot fire until previous bullet has been deleted : "
#define ITEM_BLAST_TYPE_CAUSED_BEGIN_STRING "Type of blast this bullet causes when crashing e.g. against a wall :"

#define ITEM_SPEED_CALIBRATOR_STRING "Common factor for all bullet's speed values: "
#define ITEM_DAMAGE_CALIBRATOR_STRING "Common factor for all bullet's damage values: "

  ItemPointer = LocateStringInData ( DataPointer , ITEM_SECTION_BEGIN_STRING );
  EndOfItemData = LocateStringInData ( DataPointer , ITEM_SECTION_END_STRING );

  //--------------------
  // Later, when we allow for an arbitrary amount of different items instead
  // of only the amount that fits into a fixed array, it will be useful to count
  // the number of items before, so that we can allocate the right amount of memory
  // in advance.  We count already, though it's not yet dynamic memory that is used.
  //
  Number_Of_Item_Types = CountStringOccurences ( DataPointer , NEW_ITEM_TYPE_BEGIN_STRING ) ;
  if ( Number_Of_Item_Types < ALL_ITEMS )
    {
      DebugPrintf ( 1 , "\nWe have counted %d different item types in the game data file.\n" , Number_Of_Item_Types );
    }
  else
    {
      DebugPrintf ( 1 , "\n----------------------------------------------------------------------\n
Freedroid has encountered a problem:  There seem to be more item definitions in freedroid.ruleset\n\
than the maximum allowance for item types in the ALL_ITEMS constant.\n\
Please correct the constant, recompile and restart freedroid or simply inform the developers about\n\
the problem, as usual best by sending email to freedroid-discussion@lists.sourceforge.net.\n\
We have counted %d different item types in the game data file.\n\
----------------------------------------------------------------------\n" , Number_Of_Item_Types );
    }

  //--------------------
  // Now we start to read the values for each bullet type:
  // 
  ItemPointer=DataPointer;

  while ( (ItemPointer = strstr ( ItemPointer, NEW_ITEM_TYPE_BEGIN_STRING )) != NULL)
    {
      DebugPrintf ( 1 , "\n\nFound another Item specification entry!  Lets add that to the others!");
      ItemPointer ++; // to avoid doubly taking this entry

      // Now we read in the name of this item
      ItemMap[ItemIndex].item_name = ReadAndMallocStringFromData ( ItemPointer , ITEM_NAME_INDICATION_STRING , "\"" ) ;

      // Now we read in if this item can be used by the influ without help
      YesNoString = ReadAndMallocStringFromData ( ItemPointer , ITEM_CAN_BE_APPLIED_IN_COMBAT , "\"" ) ;
      if ( strcmp( YesNoString , "yes" ) == 0 )
	{
	  ItemMap[ItemIndex].item_can_be_applied_in_combat = TRUE;
	}
      else if ( strcmp( YesNoString , "no" ) == 0 )
	{
	  ItemMap[ItemIndex].item_can_be_applied_in_combat = FALSE;
	}
      else
	{
	  fprintf(stderr, "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
The item specification of an item in freedroid.ruleset should contain an \n\
answer that is either 'yes' or 'no', but which was neither 'yes' nor 'no'.\n\
\n\
This indicated a corrupted freedroid.ruleset file with an error at least in\n\
the item specification section.  Please correct the error or send mail to the\n\
freedroid development team.\n\
\n\
But for now Freedroid will terminate to draw attention \n\
to the initialisation problem it could not resolve.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n" );
	  Terminate( ERR );
	}

      // Now we read in if this item can be installed by a mechanics bot
      YesNoString = ReadAndMallocStringFromData ( ItemPointer , ITEM_CAN_BE_INSTALLED_IN_INFLU , "\"" ) ;
      if ( strcmp( YesNoString , "yes" ) == 0 )
	{
	  ItemMap[ItemIndex].item_can_be_installed_in_influ = TRUE;
	}
      else if ( strcmp( YesNoString , "no" ) == 0 )
	{
	  ItemMap[ItemIndex].item_can_be_installed_in_influ = FALSE;
	}
      else
	{
	  fprintf(stderr, "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
The item specification of an item in freedroid.ruleset should contain an \n\
answer that is either 'yes' or 'no', but which was neither 'yes' nor 'no'.\n\
\n\
This indicated a corrupted freedroid.ruleset file with an error at least in\n\
the item specification section.  Please correct the error or send mail to the\n\
freedroid development team.\n\
\n\
But for now Freedroid will terminate to draw attention \n\
to the initialisation problem it could not resolve.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n" );
	  Terminate( ERR );
	}

      // Now we read the label telling us in which slot the item can be installed
      YesNoString = ReadAndMallocStringFromData ( ItemPointer , ITEM_CAN_BE_INSTALLED_IN_SLOT_WITH_NAME , "\"" ) ;
      ItemMap[ItemIndex].item_can_be_installed_in_weapon_slot = FALSE;
      ItemMap[ItemIndex].item_can_be_installed_in_shield_slot = FALSE;
      ItemMap[ItemIndex].item_can_be_installed_in_drive_slot = FALSE;
      ItemMap[ItemIndex].item_can_be_installed_in_armour_slot = FALSE;
      ItemMap[ItemIndex].item_can_be_installed_in_special_slot = FALSE;
      ItemMap[ItemIndex].item_can_be_installed_in_aux_slot = FALSE;
      if ( strcmp( YesNoString , "weapon" ) == 0 )
	{
	  ItemMap[ItemIndex].item_can_be_installed_in_weapon_slot = TRUE;
	}
      else if ( strcmp( YesNoString , "drive" ) == 0 )
	{
	  ItemMap[ItemIndex].item_can_be_installed_in_drive_slot = TRUE;
	}
      else if ( strcmp( YesNoString , "shield" ) == 0 )
	{
	  ItemMap[ItemIndex].item_can_be_installed_in_shield_slot = TRUE;
	}
      else if ( strcmp( YesNoString , "armour" ) == 0 )
	{
	  ItemMap[ItemIndex].item_can_be_installed_in_armour_slot = TRUE;
	}
      else if ( strcmp( YesNoString , "special" ) == 0 )
	{
	  ItemMap[ItemIndex].item_can_be_installed_in_special_slot = TRUE;
	}
      else if ( strcmp( YesNoString , "aux" ) == 0 )
	{
	  ItemMap[ItemIndex].item_can_be_installed_in_aux_slot = TRUE;
	}
      else if ( strcmp( YesNoString , "none" ) == 0 )
	{
	  // good.  Everything is ok, as long as at least 'none' was found
	}
      else
	{
	  fprintf(stderr, "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
The item specification of an item in freedroid.ruleset should contain an \n\
answer for the slot installation possiblieties, that was neither 
'weapon' nor 'armour' nor 'shield' nor 'aux' nor 'special' nor 'drive' nor 'none'.\n\
\n\
This indicated a corrupted freedroid.ruleset file with an error at least in\n\
the item specification section.  \n\
\n\
The item specification number, where the error occured was: %d.\n\
\n\
Please correct the error or send mail to the freedroid development team.\n\
\n\
But for now Freedroid will terminate to draw attention \n\
to the initialisation problem it could not resolve.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n" , ItemIndex );
	  Terminate( ERR );
	}

      // Now we read in if this item can bought in shops
      YesNoString = ReadAndMallocStringFromData ( ItemPointer , ITEM_CAN_BE_BOUGHT_IN_SHOP , "\"" ) ;
      if ( strcmp( YesNoString , "yes" ) == 0 )
	{
	  ItemMap[ItemIndex].item_can_be_bought_in_shop = TRUE;
	}
      else if ( strcmp( YesNoString , "no" ) == 0 )
	{
	  ItemMap[ItemIndex].item_can_be_bought_in_shop = FALSE;
	}
      else
	{
	  fprintf(stderr, "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
The item specification of an item in freedroid.ruleset should contain an \n\
answer that is either 'yes' or 'no', but which was neither 'yes' nor 'no'.\n\
\n\
This indicated a corrupted freedroid.ruleset file with an error at least in\n\
the item specification section.  Please correct the error or send mail to the\n\
freedroid development team.\n\
\n\
But for now Freedroid will terminate to draw attention \n\
to the initialisation problem it could not resolve.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n" );
	  Terminate( ERR );
	}

      //--------------------
      // Now we read in minimum strength, dex and magic required to wear/wield this item
      //
      ReadValueFromString( ItemPointer , "Strength minimum required to wear/wield this item=" , "%d" , 
			   &ItemMap[ItemIndex].item_require_strength , EndOfItemData );
      ReadValueFromString( ItemPointer , "Dexterity minimum required to wear/wield this item=" , "%d" , 
			   &ItemMap[ItemIndex].item_require_dexterity , EndOfItemData );
      ReadValueFromString( ItemPointer , "Magic minimum required to wear/wield this item=" , "%d" , 
			   &ItemMap[ItemIndex].item_require_magic , EndOfItemData );

      //--------------------
      // If the item is a drive, we read in the drive specification...
      // If the item isn't a drive, then we set the default values, just to be sure..
      //
      if ( ItemMap[ItemIndex].item_can_be_installed_in_drive_slot == TRUE )
	{
	  // Now we read in the maxspeed you can go with this item as drive
	  ReadValueFromString( ItemPointer ,  "Item drive maxspeed=" , "%lf" , 
			       &ItemMap[ItemIndex].item_drive_maxspeed , EndOfItemData );
	  // Now we read in the acceleration you will have with this item as drive
	  ReadValueFromString( ItemPointer ,  "Item drive acceleration=" , "%lf" , 
			       &ItemMap[ItemIndex].item_drive_accel , EndOfItemData );
	}
      else
	{
	  ItemMap[ItemIndex].item_drive_maxspeed = 0 ;
	  ItemMap[ItemIndex].item_drive_accel = 0 ;
	}

      //--------------------
      // If the item is a gun, we read in the drive specification...
      //
      if ( ItemMap[ItemIndex].item_can_be_installed_in_weapon_slot == TRUE )
	{
	  // Now we read in the damage bullets from this gun will do
	  ReadValueFromString( ItemPointer ,  "Item as gun: damage of bullets=" , "%d" , 
			       &ItemMap[ItemIndex].base_item_gun_damage , EndOfItemData );
	  ReadValueFromString( ItemPointer ,  "Item as gun: modifier for damage of bullets=" , "%d" , 
			       &ItemMap[ItemIndex].item_gun_damage_modifier , EndOfItemData );
	  
	  // Now we read in the speed this bullet will go
	  ReadValueFromString( ItemPointer ,  "Item as gun: speed of bullets=" , "%lf" , 
			       &ItemMap[ItemIndex].item_gun_speed , EndOfItemData );
	  
	  // Now we read in speed of melee application and melee offset from influ
	  ReadValueFromString( ItemPointer ,  "Item as gun: angle change of bullets=" , "%lf" , 
			       &ItemMap[ItemIndex].item_gun_angle_change , EndOfItemData );
	  ReadValueFromString( ItemPointer ,  "Item as gun: offset for melee weapon=" , "%lf" , 
			       &ItemMap[ItemIndex].item_gun_fixed_offset , EndOfItemData );
	  ReadValueFromString( ItemPointer ,  "Item as gun: modifier for starting angle=" , "%lf" , 
			       &ItemMap[ItemIndex].item_gun_start_angle_modifier , EndOfItemData );

	  // Now we read in if this weapon can pass through walls or not...
	  YesNoString = ReadAndMallocStringFromData ( ItemPointer , ITEM_GUN_IGNORE_WALL , "\"" ) ;
	  if ( strcmp( YesNoString , "yes" ) == 0 )
	    {
	      ItemMap[ItemIndex].item_gun_bullet_ignore_wall_collisions = TRUE;
	    }
	  else if ( strcmp( YesNoString , "no" ) == 0 )
	    {
	      ItemMap[ItemIndex].item_gun_bullet_ignore_wall_collisions = FALSE;
	    }
	  else
	    {
	      fprintf(stderr, "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
The item specification of an item in freedroid.ruleset should contain an \n\
answer that is either 'yes' or 'no', but which was neither 'yes' nor 'no'.\n\
\n\
This indicated a corrupted freedroid.ruleset file with an error at least in\n\
the item specification section.  Please correct the error or send mail to the\n\
freedroid development team.\n\
\n\
But for now Freedroid will terminate to draw attention \n\
to the initialisation problem it could not resolve.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n" );
	      Terminate( ERR );
	    }; // if ( ItemMap[ItemIndex].item_can_be_installed_in_weapon_slot == TRUE )
	  
	  // Now we read in if this weapons bullets will reflect other bullets or not
	  YesNoString = ReadAndMallocStringFromData ( ItemPointer , "Item as gun: reflect other bullets=\"" , "\"" ) ;
	  if ( strcmp( YesNoString , "yes" ) == 0 )
	    {
	      ItemMap[ItemIndex].item_gun_bullet_reflect_other_bullets = TRUE;
	    }
	  else if ( strcmp( YesNoString , "no" ) == 0 )
	    {
	      ItemMap[ItemIndex].item_gun_bullet_reflect_other_bullets = FALSE;
	    }
	  else
	    {
	      fprintf(stderr, "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
The item specification of an item in freedroid.ruleset should contain an \n\
answer that is either 'yes' or 'no', but which was neither 'yes' nor 'no'.\n\
\n\
This indicated a corrupted freedroid.ruleset file with an error at least in\n\
the item specification section.  Please correct the error or send mail to the\n\
freedroid development team.\n\
\n\
But for now Freedroid will terminate to draw attention \n\
to the initialisation problem it could not resolve.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n" );
	      Terminate( ERR );
	    }; // if ( ItemMap[ItemIndex].item_can_be_installed_in_weapon_slot == TRUE )
	  
	  // Now we read in if this weapons bullets will reflect other bullets or not
	  YesNoString = ReadAndMallocStringFromData ( ItemPointer , "Item as gun: pass through explosions=\"" , "\"" ) ;
	  if ( strcmp( YesNoString , "yes" ) == 0 )
	    {
	      ItemMap[ItemIndex].item_gun_bullet_pass_through_explosions = TRUE;
	    }
	  else if ( strcmp( YesNoString , "no" ) == 0 )
	    {
	      ItemMap[ItemIndex].item_gun_bullet_pass_through_explosions = FALSE;
	    }
	  else
	    {
	      fprintf(stderr, "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
The item specification of an item in freedroid.ruleset should contain an \n\
answer that is either 'yes' or 'no', but which was neither 'yes' nor 'no'.\n\
\n\
This indicated a corrupted freedroid.ruleset file with an error at least in\n\
the item specification section.  Please correct the error or send mail to the\n\
freedroid development team.\n\
\n\
But for now Freedroid will terminate to draw attention \n\
to the initialisation problem it could not resolve.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n" );
	      Terminate( ERR );
	    }; // if ( ItemMap[ItemIndex].item_can_be_installed_in_weapon_slot == TRUE )
	  
	  // Now we read in if this weapons bullets will reflect other bullets or not
	  YesNoString = ReadAndMallocStringFromData ( ItemPointer , "Item as gun: pass through hit bodies=\"" , "\"" ) ;
	  if ( strcmp( YesNoString , "yes" ) == 0 )
	    {
	      ItemMap[ItemIndex].item_gun_bullet_pass_through_hit_bodies = TRUE;
	    }
	  else if ( strcmp( YesNoString , "no" ) == 0 )
	    {
	      ItemMap[ItemIndex].item_gun_bullet_pass_through_hit_bodies = FALSE;
	    }
	  else
	    {
	      fprintf(stderr, "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
The item specification of an item in freedroid.ruleset should contain an \n\
answer that is either 'yes' or 'no', but which was neither 'yes' nor 'no'.\n\
\n\
This indicated a corrupted freedroid.ruleset file with an error at least in\n\
the item specification section.  Please correct the error or send mail to the\n\
freedroid development team.\n\
\n\
But for now Freedroid will terminate to draw attention \n\
to the initialisation problem it could not resolve.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n" );
	      Terminate( ERR );
	    }; // if ( ItemMap[ItemIndex].item_can_be_installed_in_weapon_slot == TRUE )
	  
	  // Now we read in the recharging time this weapon will need
	  ReadValueFromString( ItemPointer ,  "Item as gun: recharging time=" , "%lf" , 
			       &ItemMap[ItemIndex].item_gun_recharging_time , EndOfItemData );
	  
	  // Now we read in the image type that should be generated for this bullet
	  ReadValueFromString( ItemPointer ,  "Item as gun: bullet_image_type=" , "%d" , 
			       &ItemMap[ItemIndex].item_gun_bullet_image_type , EndOfItemData );
	  
	  // Now we read in the image type that should be generated for this bullet
	  ReadValueFromString( ItemPointer ,  "Item as gun: bullet_lifetime=" , "%lf" , 
			       &ItemMap[ItemIndex].item_gun_bullet_lifetime , EndOfItemData );
	}


	  // Now we read in the armour value of this item as armour or shield or whatever
      ReadValueFromString( ItemPointer ,  "Item as defensive item: base_ac_bonus=" , "%d" , 
			   &ItemMap[ItemIndex].base_ac_bonus , EndOfItemData );
      ReadValueFromString( ItemPointer ,  "Item as defensive item: ac_bonus_modifier=" , "%d" , 
			   &ItemMap[ItemIndex].ac_bonus_modifier , EndOfItemData );

      // Now we read in the base item duration and the duration modifier
      ReadValueFromString( ItemPointer ,  "Base item duration=" , "%d" , 
			   &ItemMap[ItemIndex].base_item_duration , EndOfItemData );
      ReadValueFromString( ItemPointer ,  "plus duration modifier=" , "%d" , 
			   &ItemMap[ItemIndex].item_duration_modifier , EndOfItemData );

      

      // Now we read in the number of the picture to be used for this item
      ReadValueFromString( ItemPointer ,  "Picture number=" , "%d" , 
			   &ItemMap[ItemIndex].picture_number , EndOfItemData );

      // Now we read in the number of the sound to be used for this item
      ReadValueFromString( ItemPointer ,  "Sound number=" , "%d" , 
			   &ItemMap[ItemIndex].sound_number , EndOfItemData );

      // Now we read in the base list price for this item
      ReadValueFromString( ItemPointer ,  "Base list price=" , "%d" , 
			   &ItemMap[ItemIndex].base_list_price , EndOfItemData );

      // Now we read in the description string of this item
      ItemMap[ItemIndex].item_description = ReadAndMallocStringFromData ( ItemPointer , ITEM_DESCRIPTION_INDICATION_STRING , "\"" ) ;

      ItemIndex++;

    }

}; // void Get_Item_Data ( char* DataPointer );


/* ----------------------------------------------------------------------
 * This function reads in the game events, i.e. the locations and conditions
 * under which some actions are triggered.
 * ---------------------------------------------------------------------- */
void 
Get_Game_Events ( char* EventSectionPointer )
{
  char *EventPointer;
  char *EndOfEvent;
  int i;
  int EventActionNumber;
  int EventTriggerNumber;

#define EVENT_TRIGGER_BEGIN_STRING "* Start of an Event Trigger Subsection *"
#define EVENT_TRIGGER_END_STRING "* End of this Event Trigger Subsection *"
#define EVENT_ACTION_BEGIN_STRING "* Start of an Event Action Subsection *"
#define EVENT_ACTION_END_STRING "* End of this Event Action Subsection *"

#define EVENT_ACTION_MAPCHANGE_POS_X_STRING "Mapchange at X="
#define EVENT_ACTION_MAPCHANGE_POS_Y_STRING " Y="
#define EVENT_ACTION_MAPCHANGE_MAPLEVEL_STRING " Lev="
#define EVENT_ACTION_MAPCHANGE_TO_WHAT_STRING " to new value="

#define EVENT_ACTION_TELEPORT_POS_X_STRING "Teleport to TelX="
#define EVENT_ACTION_TELEPORT_POS_Y_STRING " TelY="
#define EVENT_ACTION_TELEPORT_LEVEL_STRING " TelLev="

#define EVENT_ACTION_INFLUENCER_SAY_TEXT "Action is Influencer say=\""
#define EVENT_ACTION_ASSIGN_WHICH_MISSION "Action is mission assignment="
#define ACTION_LABEL_INDICATION_STRING "Action label for this action=\""

#define EVENT_TRIGGER_POS_X_STRING "Influencer must be at X="
#define EVENT_TRIGGER_POS_Y_STRING " Y="
#define EVENT_TRIGGER_POS_MAPLEVEL_STRING " Lev="
  // #define EVENT_TRIGGER_POS_X_STRING "Influencer must be at x-coordinate="
  // #define EVENT_TRIGGER_POS_Y_STRING "Influencer must be at y-coordinate="
  // #define EVENT_TRIGGER_POS_MAPLEVEL_STRING "Influencer must be at maplevel="
#define EVENT_TRIGGER_DELETED_AFTER_TRIGGERING "Delete the event trigger after it has been triggered="
#define TRIGGER_WHICH_TARGET_LABEL "Event Action to be triggered by this trigger=\""


  // Delete all events and event triggers
  for ( i = 0 ; i < MAX_EVENT_TRIGGERS ; i++ )
    {
      AllEventTriggers[i].Influ_Must_Be_At_Level=-1;
      AllEventTriggers[i].Influ_Must_Be_At_Point.x=-1;
      AllEventTriggers[i].Influ_Must_Be_At_Point.y=-1;
      
      // Maybe the event is triggered by time
      AllEventTriggers[i].Mission_Time_Must_Have_Passed=-1;
      AllEventTriggers[i].Mission_Time_Must_Not_Have_Passed=-1;
      
      // And now of course which event to trigger!!!!
      // Thats propably the most important information at all!!!
      // AllEventTriggers[i].EventNumber=-1;
      AllEventTriggers[i].TargetActionLabel="none";
    }
  for ( i = 0 ; i < MAX_TRIGGERED_ACTIONS_IN_GAME ; i++ )
    {
      // Maybe the triggered event consists of the influencer saying something
      AllTriggeredActions[i].ActionLabel="";
      AllTriggeredActions[i].InfluencerSayText="";
      // Maybe the triggered event consists of the map beeing changed at some tile
      AllTriggeredActions[i].ChangeMapLevel=-1;
      AllTriggeredActions[i].ChangeMapLocation.x=-1;
      AllTriggeredActions[i].ChangeMapLocation.y=-1;
      AllTriggeredActions[i].ChangeMapTo=-1;
      AllTriggeredActions[i].AssignWhichMission=-1;
      // Maybe the triggered event consists of ??????
    }


  //--------------------
  // At first we decode ALL THE EVENT ACTIONS not the TRIGGERS!!!!
  //
  EventPointer=EventSectionPointer;
  EventActionNumber=0;
  while ( ( EventPointer = strstr ( EventPointer , EVENT_ACTION_BEGIN_STRING ) ) != NULL)
    {
      DebugPrintf(1, "\nBegin of a new Event Action Section found. Good. ");
      EventPointer += strlen( EVENT_ACTION_BEGIN_STRING ) + 1;

      EndOfEvent = LocateStringInData ( EventSectionPointer , EVENT_ACTION_END_STRING );

      DebugPrintf (1, "\n\nStarting to read details of this event action section\n\n");

      //--------------------
      // Now we decode the details of this event action section
      //

      // FIRST OF ALL, WE NEED TO KNOW AT WHICH INDEX WE MUST MODIFY OUR STRUTURE.
      // SO FIRST WE READ IN THE EVENT ACTIONS INDEX NUMBER
      // ReadValueFromString( EventPointer , EVENT_ACTION_INDEX_NUMBER_TO_USE_STRING , "%d" , 
      // &EventActionNumber , EndOfEvent );

      AllTriggeredActions[ EventActionNumber].ActionLabel =
	ReadAndMallocStringFromData ( EventPointer , ACTION_LABEL_INDICATION_STRING , "\"" ) ;

      // Now we read in the map changing position in x and y and level coordinates
      ReadValueFromString( EventPointer , EVENT_ACTION_MAPCHANGE_POS_X_STRING , "%d" , 
			   &AllTriggeredActions[ EventActionNumber ].ChangeMapLocation.x , EndOfEvent );
      ReadValueFromString( EventPointer , EVENT_ACTION_MAPCHANGE_POS_Y_STRING , "%d" , 
			   &AllTriggeredActions[ EventActionNumber ].ChangeMapLocation.y , EndOfEvent );
      ReadValueFromString( EventPointer , EVENT_ACTION_MAPCHANGE_MAPLEVEL_STRING , "%d" , 
			   &AllTriggeredActions[ EventActionNumber ].ChangeMapLevel , EndOfEvent );

      // Now we read in the teleport target position in x and y and level coordinates
      ReadValueFromString( EventPointer , EVENT_ACTION_TELEPORT_POS_X_STRING , "%d" , 
			   &AllTriggeredActions[ EventActionNumber ].TeleportTarget.x , EndOfEvent );
      ReadValueFromString( EventPointer , EVENT_ACTION_TELEPORT_POS_Y_STRING , "%d" , 
			   &AllTriggeredActions[ EventActionNumber ].TeleportTarget.y , EndOfEvent );
      ReadValueFromString( EventPointer , EVENT_ACTION_TELEPORT_LEVEL_STRING , "%d" , 
			   &AllTriggeredActions[ EventActionNumber ].TeleportTargetLevel , EndOfEvent );

      // Now we read in the new value for that map tile
      ReadValueFromString( EventPointer , EVENT_ACTION_MAPCHANGE_TO_WHAT_STRING , "%d" , 
			   &AllTriggeredActions[ EventActionNumber ].ChangeMapTo , EndOfEvent );

      // Now we read in if the text for the influencer to say
      AllTriggeredActions[ EventActionNumber].InfluencerSayText =
	ReadAndMallocStringFromData ( EventPointer , EVENT_ACTION_INFLUENCER_SAY_TEXT , "\"" ) ;

      // Now we read in the new mission to assign to the influencer on that event 
      ReadValueFromString( EventPointer , EVENT_ACTION_ASSIGN_WHICH_MISSION , "%d" , 
			   &AllTriggeredActions[ EventActionNumber ].AssignWhichMission , EndOfEvent );

      EventActionNumber++;
    } // While Event action begin string found...


  DebugPrintf (1, "\nThat must have been the last Event Action section.\nWe can now start with the Triggers. Good.");  


  //----------------------------------------------------------------------

  //--------------------
  // Now we decode ALL THE EVENT TRIGGERS not the ACTIONS!!!!
  //
  EventPointer=EventSectionPointer;
  EventTriggerNumber=0;
  while ( ( EventPointer = strstr ( EventPointer , EVENT_TRIGGER_BEGIN_STRING ) ) != NULL)
    {
      DebugPrintf(1, "\nBegin of a new Event Trigger Section found. Good. ");
      EventPointer += strlen( EVENT_TRIGGER_BEGIN_STRING ) + 1;

      EndOfEvent = LocateStringInData ( EventSectionPointer , EVENT_TRIGGER_END_STRING );

      DebugPrintf ( 1 , "\nStarting to read details of this event trigger section\n\n");

      //--------------------
      // Now we decode the details of this event trigger section
      //

      // Now we read in the triggering position in x and y coordinates
      ReadValueFromString( EventPointer , EVENT_TRIGGER_POS_X_STRING , "%d" , 
			   &AllEventTriggers[ EventTriggerNumber ].Influ_Must_Be_At_Point.x , EndOfEvent );
      ReadValueFromString( EventPointer , EVENT_TRIGGER_POS_Y_STRING , "%d" , 
			   &AllEventTriggers[ EventTriggerNumber ].Influ_Must_Be_At_Point.y , EndOfEvent );

      // Now we read in the triggering position in levels
      ReadValueFromString( EventPointer , EVENT_TRIGGER_POS_MAPLEVEL_STRING , "%d" , 
			   &AllEventTriggers[ EventTriggerNumber ].Influ_Must_Be_At_Level , EndOfEvent );

      // Now we read whether or not to delete the trigger after being triggerd
      ReadValueFromString( EventPointer , EVENT_TRIGGER_DELETED_AFTER_TRIGGERING , "%d" , 
			   &AllEventTriggers[ EventTriggerNumber ].DeleteTriggerAfterExecution , EndOfEvent );

      // Now we read in the action to be invoked by this trigger
      // ReadValueFromString( EventPointer , EVENT_TRIGGER_WHICH_ACTION_STRING , "%d" , 
      // &AllEventTriggers[ EventTriggerNumber ].EventNumber , EndOfEvent );
      AllEventTriggers[ EventTriggerNumber ].TargetActionLabel = 
	ReadAndMallocStringFromData ( EventPointer , TRIGGER_WHICH_TARGET_LABEL , "\"" ) ;

      EventTriggerNumber++;
    } // While Event trigger begin string found...


  DebugPrintf (1 , "\nThat must have been the last Event Trigger section.");

}; // void Get_Game_Events ( char* EventSectionPointer );


/* ----------------------------------------------------------------------
 * This function loads all the constant concerning robot archetypes
 * from a section in memory to the actual archetype structures.
 * ---------------------------------------------------------------------- */
void
Get_Robot_Data ( void* DataPointer )
{
  int RobotIndex = 0;
  char *RobotPointer;
  char *EndOfDataPointer;
  int i;

  double maxspeed_calibrator;
  double acceleration_calibrator;
  double maxenergy_calibrator;
  double energyloss_calibrator;
  double aggression_calibrator;
  double score_calibrator;

#define MAXSPEED_CALIBRATOR_STRING "Common factor for all droids maxspeed values: "
#define ACCELERATION_CALIBRATOR_STRING "Common factor for all droids acceleration values: "
#define MAXENERGY_CALIBRATOR_STRING "Common factor for all droids maximum energy values: "
#define ENERGYLOSS_CALIBRATOR_STRING "Common factor for all droids energyloss values: "
#define AGGRESSION_CALIBRATOR_STRING "Common factor for all droids aggression values: "
#define SCORE_CALIBRATOR_STRING "Common factor for all droids score values: "


#define ROBOT_SECTION_BEGIN_STRING "*** Start of Robot Data Section: ***" 
#define ROBOT_SECTION_END_STRING "*** End of Robot Data Section: ***" 
#define NEW_ROBOT_BEGIN_STRING "** Start of new Robot: **" 
#define DROIDNAME_BEGIN_STRING "Droidname: "
#define PORTRAIT_FILENAME_WITHOUT_EXT "Droid portrait file name (without extension) to use=\""
#define MAXSPEED_BEGIN_STRING "Maximum speed of this droid: "
#define CLASS_BEGIN_STRING "Class of this droid: "
#define ACCELERATION_BEGIN_STRING "Maximum acceleration of this droid: "
#define MAXENERGY_BEGIN_STRING "Maximum energy of this droid: "
#define MAXMANA_BEGIN_STRING "Maximum mana of this droid: "
#define LOSEHEALTH_BEGIN_STRING "Rate of energyloss under influence control: "
#define GUN_BEGIN_STRING "Weapon type this droid uses: "
#define AGGRESSION_BEGIN_STRING "Aggression rate of this droid: "
#define FLASHIMMUNE_BEGIN_STRING "Is this droid immune to disruptor blasts? "
#define SCORE_BEGIN_STRING "Score gained for destroying one of this type: "
#define HEIGHT_BEGIN_STRING "Height of this droid : "
#define WEIGHT_BEGIN_STRING "Weight of this droid : "
#define DRIVE_BEGIN_STRING "Drive of this droid : "
#define BRAIN_BEGIN_STRING "Brain of this droid : "
#define SENSOR1_BEGIN_STRING "Sensor 1 of this droid : "
#define SENSOR2_BEGIN_STRING "Sensor 2 of this droid : "
#define SENSOR3_BEGIN_STRING "Sensor 3 of this droid : "
#define ARMAMENT_BEGIN_STRING "Armament of this droid : "
#define DRIVE_ITEM_BEGIN_STRING "Drive item="
#define WEAPON_ITEM_BEGIN_STRING "Weapon item="
#define SHIELD_ITEM_BEGIN_STRING "Shield item="
#define ARMOUR_ITEM_BEGIN_STRING "Armour item="
#define AUX1_ITEM_BEGIN_STRING "Aux1 item="
#define AUX2_ITEM_BEGIN_STRING "Aux2 item="
#define SPECIAL_ITEM_BEGIN_STRING "Special item="
#define GREETING_SOUND_STRING "Greeting Sound number="
#define ENEMY_GOT_HIT_SOUND_STRING "Got Hit Sound number="
#define TO_HIT_STRING "Chance of this robot scoring a hit="
#define GETTING_HIT_MODIFIER_STRING "Chance modifier, that this robot gets hit="
#define ADVANCED_FIGHTING_BEGIN_STRING "Advanced Fighting present in this droid : "
#define IS_HUMAN_SPECIFICATION_STRING "Is this 'droid' a human : "
#define INDIVIDUAL_SHAPE_SPECIFICATION_STRING "Individual shape of this droid or just -1 for classic ball shaped : "
#define GO_REQUEST_REINFORCEMENTS_BEGIN_STRING "Going to request reinforcements typical for this droid : "
#define NOTES_BEGIN_STRING "Notes concerning this droid : "

  
  RobotPointer = LocateStringInData ( DataPointer , ROBOT_SECTION_BEGIN_STRING );
  EndOfDataPointer = LocateStringInData ( DataPointer , ROBOT_SECTION_END_STRING );

  
  DebugPrintf (2, "\n\nStarting to read robot calibration section\n\n");

  // Now we read in the speed calibration factor for all droids
  ReadValueFromString( RobotPointer , MAXSPEED_CALIBRATOR_STRING , "%lf" , 
		       &maxspeed_calibrator , EndOfDataPointer );

  // Now we read in the acceleration calibration factor for all droids
  ReadValueFromString( RobotPointer , ACCELERATION_CALIBRATOR_STRING , "%lf" , 
		       &acceleration_calibrator , EndOfDataPointer );

  // Now we read in the maxenergy calibration factor for all droids
  ReadValueFromString( RobotPointer , MAXENERGY_CALIBRATOR_STRING , "%lf" , 
		       &maxenergy_calibrator , EndOfDataPointer );

  // Now we read in the energy_loss calibration factor for all droids
  ReadValueFromString( RobotPointer , ENERGYLOSS_CALIBRATOR_STRING , "%lf" , 
		       &energyloss_calibrator , EndOfDataPointer );

  // Now we read in the aggression calibration factor for all droids
  ReadValueFromString( RobotPointer , AGGRESSION_CALIBRATOR_STRING , "%lf" , 
		       &aggression_calibrator , EndOfDataPointer );

  // Now we read in the score calibration factor for all droids
  ReadValueFromString( RobotPointer , SCORE_CALIBRATOR_STRING , "%lf" , 
		       &score_calibrator , EndOfDataPointer );

  DebugPrintf ( 1 , "\n\nStarting to read Robot data...\n\n" );
  //--------------------
  // At first, we must allocate memory for the droid specifications.
  // How much?  That depends on the number of droids defined in freedroid.ruleset.
  // So we have to count those first.  ok.  lets do it.

  Number_Of_Droid_Types = CountStringOccurences ( DataPointer , NEW_ROBOT_BEGIN_STRING ) ;

  // Not that we know how many robots are defined in freedroid.ruleset, we can allocate
  // a fitting amount of memory.
  i=sizeof(druidspec);
  Druidmap = MyMalloc ( i * (Number_Of_Droid_Types + 1) + 1 );
  DebugPrintf(1, "\nWe have counted %d different druid types in the game data file." , Number_Of_Droid_Types );
  DebugPrintf (2, "\nMEMORY HAS BEEN ALLOCATED.\nTHE READING CAN BEGIN.\n" );

  //--------------------
  //Now we start to read the values for each robot:
  //Of which parts is it composed, which stats does it have?
  while ( (RobotPointer = strstr ( RobotPointer, NEW_ROBOT_BEGIN_STRING )) != NULL)
    {
      DebugPrintf (2, "\n\nFound another Robot specification entry!  Lets add that to the others!");
      RobotPointer ++; // to avoid doubly taking this entry

      // Now we read in the Name of this droid.  We consider as a name the rest of the
      // line with the DROIDNAME_BEGIN_STRING until the "\n" is found.
      Druidmap[RobotIndex].druidname =
	ReadAndMallocStringFromData ( RobotPointer , DROIDNAME_BEGIN_STRING , "\n" ) ;

      // Now we read in the file name of the portrait file for this droid.  
      // Is should be enclosed in double-quotes.
      Druidmap[RobotIndex].portrait_filename_without_ext =
	ReadAndMallocStringFromData ( RobotPointer , PORTRAIT_FILENAME_WITHOUT_EXT , "\"" ) ;

      // #define PORTRAIT_FILENAME_WITHOUT_EXT "Droid portrait file name (without extension) to use=\""

      // Now we read in the maximal speed this droid can go. 
      // ReadValueFromString( RobotPointer , MAXSPEED_BEGIN_STRING , "%lf" , 
      // &Druidmap[RobotIndex].maxspeed , EndOfDataPointer );

      // Now we read in the class of this droid.
      ReadValueFromString( RobotPointer , CLASS_BEGIN_STRING , "%d" , 
			   &Druidmap[RobotIndex].class , EndOfDataPointer );

      // Now we read in the maximal acceleration this droid can go. 
      // ReadValueFromString( RobotPointer , ACCELERATION_BEGIN_STRING , "%lf" , 
      // &Druidmap[RobotIndex].accel , EndOfDataPointer );

      // Now we read in the maximal energy this droid can store. 
      ReadValueFromString( RobotPointer , MAXENERGY_BEGIN_STRING , "%lf" , 
			   &Druidmap[RobotIndex].maxenergy , EndOfDataPointer );

      // Now we read in the maximal mana this droid can store. 
      ReadValueFromString( RobotPointer , MAXMANA_BEGIN_STRING , "%lf" , 
			   &Druidmap[RobotIndex].maxmana , EndOfDataPointer );

      // Now we read in the lose_health rate.
      ReadValueFromString( RobotPointer , LOSEHEALTH_BEGIN_STRING , "%lf" , 
			   &Druidmap[RobotIndex].lose_health , EndOfDataPointer );

      // Now we read in the class of this droid.
      // ReadValueFromString( RobotPointer , GUN_BEGIN_STRING , "%d" , 
      // &Druidmap[RobotIndex].gun , EndOfDataPointer );

      // Now we read in the aggression rate of this droid.
      ReadValueFromString( RobotPointer , AGGRESSION_BEGIN_STRING , "%d" , 
			   &Druidmap[RobotIndex].aggression , EndOfDataPointer );

      // Now we read in the flash immunity of this droid.
      ReadValueFromString( RobotPointer , FLASHIMMUNE_BEGIN_STRING , "%d" , 
			   &Druidmap[RobotIndex].flashimmune , EndOfDataPointer );

      // Now we score to be had for destroying one droid of this type
      ReadValueFromString( RobotPointer , SCORE_BEGIN_STRING , "%d" , 
			   &Druidmap[RobotIndex].score, EndOfDataPointer );

      // Now we read in the monster level = maximum treasure chest to pick from
      ReadValueFromString( RobotPointer , "Monster Level and maximum treasure chest to pick from=" , "%d" , 
			   &Druidmap[RobotIndex].monster_level , EndOfDataPointer );

      // Now we read in the number of additional magical items this monster type must drop
      ReadValueFromString( RobotPointer , "Force how many additional magic items to be dropped=" , "%d" , 
			   &Druidmap[RobotIndex].forced_magic_items , EndOfDataPointer );

      // Now we read in the height of this droid of this type
      ReadValueFromString( RobotPointer , HEIGHT_BEGIN_STRING , "%lf" , 
			   &Druidmap[RobotIndex].height, EndOfDataPointer );

      // Now we read in the weight of this droid type
      ReadValueFromString( RobotPointer , WEIGHT_BEGIN_STRING , "%lf" , 
			   &Druidmap[RobotIndex].weight, EndOfDataPointer );

      // Now we read in the drive of this droid of this type
      // ReadValueFromString( RobotPointer , DRIVE_BEGIN_STRING , "%d" , 
      // &Druidmap[RobotIndex].drive, EndOfDataPointer );

      // Now we read in the brain of this droid of this type
      ReadValueFromString( RobotPointer , BRAIN_BEGIN_STRING , "%d" , 
			   &Druidmap[RobotIndex].brain, EndOfDataPointer );

      // Now we read in the sensor 1, 2 and 3 of this droid type
      ReadValueFromString( RobotPointer , SENSOR1_BEGIN_STRING , "%d" , 
			   &Druidmap[RobotIndex].sensor1, EndOfDataPointer );
      ReadValueFromString( RobotPointer , SENSOR2_BEGIN_STRING , "%d" , 
			   &Druidmap[RobotIndex].sensor2, EndOfDataPointer );
      ReadValueFromString( RobotPointer , SENSOR3_BEGIN_STRING , "%d" , 
			   &Druidmap[RobotIndex].sensor3, EndOfDataPointer );

      // Now we read in the armament of this droid type
      // ReadValueFromString( RobotPointer , ARMAMENT_BEGIN_STRING , "%d" , 
      // &Druidmap[RobotIndex].armament , EndOfDataPointer );

      // Now we read in the drive item of this droid type
      ReadValueFromString( RobotPointer , DRIVE_ITEM_BEGIN_STRING , "%d" , 
			   &Druidmap[RobotIndex].drive_item.type , EndOfDataPointer );

      // Now we read in the weapon item of this droid type
      ReadValueFromString( RobotPointer , WEAPON_ITEM_BEGIN_STRING , "%d" , 
			   &Druidmap[RobotIndex].weapon_item.type , EndOfDataPointer );

      // Now we read in the shield item of this droid type
      ReadValueFromString( RobotPointer , SHIELD_ITEM_BEGIN_STRING , "%d" , 
			   &Druidmap[RobotIndex].shield_item.type , EndOfDataPointer );

      // Now we read in the armour item of this droid type
      ReadValueFromString( RobotPointer , ARMOUR_ITEM_BEGIN_STRING , "%d" , 
			   &Druidmap[RobotIndex].armour_item.type , EndOfDataPointer );

      // Now we read in the aux1 item of this droid type
      ReadValueFromString( RobotPointer , AUX1_ITEM_BEGIN_STRING , "%d" , 
			   &Druidmap[RobotIndex].aux1_item.type , EndOfDataPointer );

      // Now we read in the aux2 item of this droid type
      ReadValueFromString( RobotPointer , AUX2_ITEM_BEGIN_STRING , "%d" , 
			   &Druidmap[RobotIndex].aux2_item.type , EndOfDataPointer );

      // Now we read in the special item of this droid type
      ReadValueFromString( RobotPointer , SPECIAL_ITEM_BEGIN_STRING , "%d" , 
			   &Druidmap[RobotIndex].special_item.type , EndOfDataPointer );

      // Now we read in the greeting sound type of this droid type
      ReadValueFromString( RobotPointer , GREETING_SOUND_STRING , "%d" , 
			   &Druidmap[RobotIndex].greeting_sound_type , EndOfDataPointer );

      // Now we read in the greeting sound type of this droid type
      ReadValueFromString( RobotPointer , ENEMY_GOT_HIT_SOUND_STRING , "%d" , 
			   &Druidmap[RobotIndex].got_hit_sound_type , EndOfDataPointer );

      // Now we read in the to-hit chance this robot has in combat against an unarmoured target
      ReadValueFromString( RobotPointer , TO_HIT_STRING , "%d" , 
			   &Druidmap[RobotIndex].to_hit , EndOfDataPointer );

      // Now we read in the modifier, that increases/decreases the chance of this robot getting hit
      ReadValueFromString( RobotPointer , GETTING_HIT_MODIFIER_STRING , "%d" , 
			   &Druidmap[RobotIndex].getting_hit_modifier , EndOfDataPointer );

      // Now we read in the AdvancedFighing flag of this droid type
      ReadValueFromString( RobotPointer , ADVANCED_FIGHTING_BEGIN_STRING , "%d" , 
			   &Druidmap[RobotIndex].advanced_behaviour , EndOfDataPointer );

      // Now we read in the is_human flag of this droid type
      ReadValueFromString( RobotPointer , IS_HUMAN_SPECIFICATION_STRING , "%d" , 
			   &Druidmap[RobotIndex].is_human , EndOfDataPointer );

      // Now we read in the is_human flag of this droid type
      ReadValueFromString( RobotPointer , INDIVIDUAL_SHAPE_SPECIFICATION_STRING , "%d" , 
			   &Druidmap[RobotIndex].individual_shape_nr , EndOfDataPointer );

      // Now we read in if the droid tends to go to call for reinforcements
      ReadValueFromString( RobotPointer , GO_REQUEST_REINFORCEMENTS_BEGIN_STRING , "%d" , 
			   &Druidmap[RobotIndex].call_for_help_after_getting_hit , EndOfDataPointer );

      // Now we read in the notes concerning this droid.  We consider as notes all the rest of the
      // line after the NOTES_BEGIN_STRING until the "\n" is found.
      Druidmap[RobotIndex].notes = 
	ReadAndMallocStringFromData ( RobotPointer , NOTES_BEGIN_STRING , "\n" ) ;

      // Now we're potentially ready to process the next droid.  Therefore we proceed to
      // the next number in the Droidmap array.
      RobotIndex++;
    }

  DebugPrintf ( 1 , "\n\nThat must have been the last robot.  We're done reading the robot data.");
  DebugPrintf ( 1 , "\n\nApplying the calibration factors to all droids...");

  for ( i=0; i< Number_Of_Droid_Types ; i++ ) 
    {
      // Druidmap[i].maxspeed *= maxspeed_calibrator;
      // Druidmap[i].accel *= acceleration_calibrator;
      Druidmap[i].maxenergy *= maxenergy_calibrator;
      Druidmap[i].lose_health *= energyloss_calibrator;
      Druidmap[i].aggression *= aggression_calibrator;
      Druidmap[i].score *= score_calibrator;

      Druidmap[i].weapon_item.currently_held_in_hand = FALSE ;
    }
}; // int Get_Robot_Data ( void )

/* ----------------------------------------------------------------------
 * This function loads all the constant variables of the game from
 * a data file, using mainly subroutines which do the main work.
 * ---------------------------------------------------------------------- */
void
Init_Game_Data ( char * Datafilename )
{
  char *fpath;
  char *Data;

#define END_OF_GAME_DAT_STRING "*** End of game.dat File ***"

  DebugPrintf (2, "\nint Init_Game_Data ( char* Datafilename ) called.");

  /* Read the whole game data to memory */
  fpath = find_file (Datafilename, MAP_DIR, FALSE);

  DebugPrintf ( 0 , "\nvoid Init_Game_Data:  Data will be taken from file : %s.  Commencing... \n" ,
		Datafilename );

  Data = ReadAndMallocAndTerminateFile( fpath , END_OF_GAME_DAT_STRING ) ;

  Get_General_Game_Constants( Data );

  Get_Item_Data ( Data );

  Get_Robot_Data ( Data );

  Get_Bullet_Data ( Data );

  // free ( Data ); DO NOT FREE THIS AREA UNLESS YOU REALLOCATE MEMORY FOR THE
  // DROIDNAMES EVERY TIME!!!

} // int Init_Game_Data ( void )

/* -----------------------------------------------------------------
 * This function is for stability while working with the SVGALIB, which otherwise would
 * be inconvenient if not dangerous in the following respect:  When SVGALIB has switched to
 * graphic mode and has grabbed the keyboard in raw mode and the program gets stuck, the 
 * console will NOT be returned to normal, the keyboard will remain useless and login from
 * outside and shutting down or reseting the console will be the only way to avoid a hard
 * reset!
 * Therefore this function is introduced.  When Paradroid starts up, the operating system is
 * instructed to generate a signal ALARM after a specified time has passed.  This signal will
 * be handled by this function, which in turn restores to console to normal and resets the
 * yiff sound server access if applicable. (All this is done via calling Terminate
 * of course.) 
 * -----------------------------------------------------------------*/
static void
timeout (int sig)
{
  DebugPrintf (2, "\n\nstatic void timeout(int sig): Automatic termination NOW!!");
  Terminate (0);
}				/* timeout */

char copyright[] = "\nCopyright (C) 2002 Johannes Prix, Reinhard Prix\n\
Freedroid comes with NO WARRANTY to the extent permitted by law.\n\
You may redistribute copies of Freedroid\n\
under the terms of the GNU General Public License.\n\
For more information about these matters, see the file named COPYING.\n";


char usage_string[] =
  "Usage: freedroid [-v|--version] \n\
                    [-q|--nosound] \n\
                    [-s|--sound] \n\
                    [-f|--fullscreen] [-w|--window]\n\
                    [-j|--sensitivity]\n\
                    [-d|--debug=LEVEL]\n\
\n\
Please report bugs on our sourceforge-website:\n\
http://sourceforge.net/projects/freedroid/\n\n";

/* -----------------------------------------------------------------
 *  parse command line arguments and set global switches 
 *  exit on error, so we don't need to return success status
 * -----------------------------------------------------------------*/
void
parse_command_line (int argc, char *const argv[])
{
  int c;
  int timeout_time;		/* timeout to restore text-mode */

  static struct option long_options[] = {
    {"version", 0, 0, 'v'},
    {"help", 	0, 0, 'h'},
    {"nosound", 0, 0, 'q'},
    {"sound", 	0, 0, 's'},
    {"timeout", 1, 0, 't'},
    {"debug", 	2, 0, 'd'},
    {"window",  0, 0, 'w'},
    {"fullscreen",0,0,'f'},
    {"sensitivity",1,0,'j'},
    { 0, 	0, 0,  0}
  };

  while (1)
    {
      c = getopt_long (argc, argv, "vqst:h?d::wfj:", long_options, NULL);
      if (c == -1)
	break;

      switch (c)
	{
	  /* version statement -v or --version
	   * following gnu-coding standards for command line interfaces */
	case 'v':
	  printf ("\n%s %s  \n", PACKAGE, VERSION);
	  printf (copyright);
	  exit (0);
	  break;

	case 'h':
	case '?':
	  printf (usage_string);
	  exit (0);
	  break;

	case 'q':
	  sound_on = FALSE;
	  break;

	case 's':
	  sound_on = TRUE;
	  break;

	case 't':
	  timeout_time = atoi (optarg);
	  if (timeout_time > 0)
	    {
	      signal (SIGALRM, timeout);
	      alarm (timeout_time);	/* Terminate after some seconds for safety. */
	    }
	  break;
	case 'j':
	  joy_sensitivity = atoi (optarg);
	  if (joy_sensitivity < 0 || joy_sensitivity > 32)
	    {
	      printf ("\nJoystick sensitivity must lie in the range [0;32]\n");
	      Terminate(ERR);
	    }
	  break;

	case 'd':
	  if (!optarg) 
	    debug_level = 1;
	  else
	    debug_level = atoi (optarg);
	  break;

	case 'f':
	  fullscreen_on = TRUE;
	  break;
	case 'w':
	  fullscreen_on = FALSE;
	  break;

	default:
	  printf ("\nOption %c not implemented yet! Ignored.", c);
	  break;
	}			/* switch(c) */
    }				/* while(1) */
}				/* parse_command_line */


/* ----------------------------------------------------------------------
 * This function assigns a new mission to the influencer, which means 
 * that the status of the mission in the mission array is changed and
 * perhaps the mission log activated.
 * ---------------------------------------------------------------------- */
void 
AssignMission( int MissNum )
{
  int j;

  Mission_Status_Change_Sound ( );
  GameConfig.Mission_Log_Visible = TRUE;
  GameConfig.Mission_Log_Visible_Time = 0;
  Me[0].AllMissions[ MissNum ].MissionWasAssigned = TRUE;
  
  for ( j = 0 ; j < MAX_MISSION_TRIGGERED_ACTIONS ; j ++ )
    {
      ExecuteEvent( Me[0].AllMissions[ MissNum ].ListOfActionsToBeTriggeredAtAssignment[ j ] , 0 );
    }

}; // void AssignMission( int MissNum );

/* ----------------------------------------------------------------------
 * This function reads the mission specifications from the mission file
 * which is assumed to be loaded into memory already.
 * ---------------------------------------------------------------------- */
void 
Get_Mission_Targets( char* MissionTargetPointer )
{
  char *EndOfMissionTargetPointer;
  char *NextEventPointer;
  int MissionTargetIndex = 0;
  int NumberOfEventsToTriggerAtThisAssignment;
  int NumberOfEventsToTriggerAtThisCompletition;
  int ActionNr;
  char* ActionLabel;

#define MISSION_TARGET_SUBSECTION_START_STRING "** Start of this mission target subsection **"
#define MISSION_TARGET_SUBSECTION_END_STRING "** End of this mission target subsection **"

#define MISSION_TARGET_NAME_INITIALIZER "Mission Name=\""

#define MISSION_AUTOMATICALLY_ASSIGN_STRING "Assign this mission to influencer automatically at start : "
#define MISSION_TARGET_FETCH_ITEM_STRING "Mission target is to fetch item : "
#define MISSION_TARGET_KILL_ALL_STRING "Mission target is to kill all droids : "
#define MISSION_TARGET_KILL_CLASS_STRING "Mission target is to kill class of droids : "
#define MISSION_TARGET_KILL_ONE_STRING "Mission target is to kill droids with marker : "
#define MISSION_TARGET_MUST_REACH_LEVEL_STRING "Mission target is to reach level : "
#define MISSION_TARGET_MUST_REACH_POINT_X_STRING "Mission target is to reach X-Pos : "
#define MISSION_TARGET_MUST_REACH_POINT_Y_STRING "Mission target is to reach Y-Pos : "
#define MISSION_TARGET_MUST_LIVE_TIME_STRING "Mission target is to live for how many seconds : "
#define MISSION_TARGET_MUST_BE_CLASS_STRING "Mission target is to become class : "
#define MISSION_TARGET_MUST_BE_TYPE_STRING "Mission target is to become type : "
#define MISSION_TARGET_MUST_BE_ONE_STRING "Mission target is to overtake a droid with marker : "

#define MISSION_ASSIGNMENT_TRIGGERED_ACTION_STRING "On mission assignment immediately trigger action Nr. : "
#define MISSION_COMPLETITION_TRIGGERED_ACTION_STRING "On mission completition immediately trigger action labeled=\""


  //--------------------
  // At first we clear out all existing mission entries, so that no 'zombies' remain
  // when the game is restarted and (perhaps less) new missions are loaded.
  //
  for ( MissionTargetIndex = 0 ; MissionTargetIndex < MAX_MISSIONS_IN_GAME ; MissionTargetIndex ++ )
    {
      Me[0].AllMissions[ MissionTargetIndex ].MissionExistsAtAll = FALSE;
      Me[0].AllMissions[ MissionTargetIndex ].MissionIsComplete = FALSE;
      Me[0].AllMissions[ MissionTargetIndex ].MissionWasFailed = FALSE;
      Me[0].AllMissions[ MissionTargetIndex ].MissionWasAssigned = FALSE;
    }



  MissionTargetIndex = 0;
  while ( ( MissionTargetPointer = strstr( MissionTargetPointer , MISSION_TARGET_SUBSECTION_START_STRING ) ) != NULL )
    {
      EndOfMissionTargetPointer = LocateStringInData ( MissionTargetPointer , MISSION_TARGET_SUBSECTION_END_STRING ) ;

      Me[0].AllMissions[ MissionTargetIndex ].MissionExistsAtAll = TRUE;
      Me[0].AllMissions[ MissionTargetIndex ].MissionIsComplete = FALSE;
      Me[0].AllMissions[ MissionTargetIndex ].MissionWasFailed = FALSE;
      Me[0].AllMissions[ MissionTargetIndex ].MissionWasAssigned = FALSE;

      // Me[0].AllMissions[ MissionTargetIndex ].MissionName = 
      // ReadAndMallocStringFromData ( MissionTargetPointer , MISSION_TARGET_NAME_INITIALIZER , "\"" ) ;
      strcpy ( Me[0].AllMissions[ MissionTargetIndex ].MissionName , ReadAndMallocStringFromData ( MissionTargetPointer , MISSION_TARGET_NAME_INITIALIZER , "\"" ) ) ;

      //--------------------
      // No we read in if this mission should be assigned to the influencer
      // automatically at the game start and without the influencer having to apply
      // for the mission first.
      //
      // The assignment however will take at the end of this mission list initialisation function,
      // cause we need the rest of the mission target data and the events to properly 'assign' the mission.
      // 
      ReadValueFromString( MissionTargetPointer , MISSION_AUTOMATICALLY_ASSIGN_STRING , "%d" , 
			   &Me[0].AllMissions[ MissionTargetIndex ].AutomaticallyAssignThisMissionAtGameStart , 
			   EndOfMissionTargetPointer );

      //--------------------
      // From here on we read the details of the mission target, i.e. what the
      // influencer has to do, so that the mission can be thought of as completed
      //
      ReadValueFromString( MissionTargetPointer , MISSION_TARGET_FETCH_ITEM_STRING , "%d" , 
			   &Me[0].AllMissions[ MissionTargetIndex ].fetch_item , EndOfMissionTargetPointer );

      ReadValueFromString( MissionTargetPointer , MISSION_TARGET_KILL_ALL_STRING , "%d" , 
			   &Me[0].AllMissions[ MissionTargetIndex ].KillAll , EndOfMissionTargetPointer );

      ReadValueFromString( MissionTargetPointer , MISSION_TARGET_KILL_CLASS_STRING , "%d" , 
			   &Me[0].AllMissions[ MissionTargetIndex ].KillClass , EndOfMissionTargetPointer );

      ReadValueFromString( MissionTargetPointer , MISSION_TARGET_KILL_ONE_STRING , "%d" , 
			   &Me[0].AllMissions[ MissionTargetIndex ].KillOne , EndOfMissionTargetPointer );

      ReadValueFromString( MissionTargetPointer , MISSION_TARGET_MUST_BE_CLASS_STRING , "%d" , 
			   &Me[0].AllMissions[ MissionTargetIndex ].MustBeClass , EndOfMissionTargetPointer );

      ReadValueFromString( MissionTargetPointer , MISSION_TARGET_MUST_BE_TYPE_STRING , "%d" , 
			   &Me[0].AllMissions[ MissionTargetIndex ].MustBeType , EndOfMissionTargetPointer );

      ReadValueFromString( MissionTargetPointer , MISSION_TARGET_MUST_BE_ONE_STRING , "%d" , 
			   &Me[0].AllMissions[ MissionTargetIndex ].MustBeOne , EndOfMissionTargetPointer );
      
      ReadValueFromString( MissionTargetPointer , MISSION_TARGET_MUST_REACH_POINT_X_STRING , "%d" , 
			   &Me[0].AllMissions[ MissionTargetIndex ].MustReachPoint.x , EndOfMissionTargetPointer );

      ReadValueFromString( MissionTargetPointer , MISSION_TARGET_MUST_REACH_POINT_Y_STRING , "%d" , 
			   &Me[0].AllMissions[ MissionTargetIndex ].MustReachPoint.y , EndOfMissionTargetPointer );

      ReadValueFromString( MissionTargetPointer , MISSION_TARGET_MUST_REACH_LEVEL_STRING , "%d" , 
			   &Me[0].AllMissions[ MissionTargetIndex ].MustReachLevel , EndOfMissionTargetPointer );

      ReadValueFromString( MissionTargetPointer , MISSION_TARGET_MUST_LIVE_TIME_STRING , "%lf" , 
			   &Me[0].AllMissions[ MissionTargetIndex ].MustLiveTime , EndOfMissionTargetPointer );

      //--------------------
      // At this point we have read in the target values.  Now it is time to
      // read in the events, that need to be triggered immediately after the mission has been
      // assigned.
      //
      // But first we initialize all the actions to be triggered with -1
      //
      for ( ActionNr = 0 ; ActionNr < MAX_MISSION_TRIGGERED_ACTIONS; ActionNr ++ )
	{
	  Me[0].AllMissions[ MissionTargetIndex ].ListOfActionsToBeTriggeredAtAssignment[ ActionNr ] = (-1) ;
	}

      NextEventPointer = MissionTargetPointer;
      NumberOfEventsToTriggerAtThisAssignment = 0;
      while ( ( NextEventPointer = strstr( NextEventPointer , MISSION_ASSIGNMENT_TRIGGERED_ACTION_STRING ) ) != NULL )
	{

	  ReadValueFromString( NextEventPointer , MISSION_ASSIGNMENT_TRIGGERED_ACTION_STRING , "%d" ,
			       &Me[0].AllMissions[ MissionTargetIndex ].ListOfActionsToBeTriggeredAtAssignment[ NumberOfEventsToTriggerAtThisAssignment ] ,
			       EndOfMissionTargetPointer );


	  NumberOfEventsToTriggerAtThisAssignment ++;
	  NextEventPointer ++;
	}
      DebugPrintf ( 1 , "\nDetected %d events to be triggered at this assignment." , 
		    NumberOfEventsToTriggerAtThisAssignment ) ;

      //--------------------
      // Now it is time to read in the events, that need to be triggered immediately after the
      // mission has been completed.
      //
      // But first we initialize all the actions to be triggered with -1
      //
      for ( ActionNr = 0 ; ActionNr < MAX_MISSION_TRIGGERED_ACTIONS; ActionNr ++ )
	{
	  Me[0].AllMissions[ MissionTargetIndex ].ListOfActionsToBeTriggeredAtCompletition[ ActionNr ] = (-1) ;
	}

      NextEventPointer = MissionTargetPointer;
      NumberOfEventsToTriggerAtThisCompletition = 0;
      while ( ( NextEventPointer = strstr( NextEventPointer , MISSION_COMPLETITION_TRIGGERED_ACTION_STRING ) ) != NULL )
	{

	  ActionLabel=
	    ReadAndMallocStringFromData ( NextEventPointer , MISSION_COMPLETITION_TRIGGERED_ACTION_STRING , "\"" ) ;

	  Me[0].AllMissions[ MissionTargetIndex ].ListOfActionsToBeTriggeredAtCompletition[ NumberOfEventsToTriggerAtThisAssignment ] = GiveNumberToThisActionLabel ( ActionLabel );
	  
	  NumberOfEventsToTriggerAtThisCompletition ++;
	  NextEventPointer ++;
	}
      DebugPrintf ( 1 , "\nDetected %d events to be triggered at this mission completition." , 
		    NumberOfEventsToTriggerAtThisCompletition );

      //--------------------
      // Now we are done with reading in THIS one mission target
      // We need to advance the MissionTargetPointer, so that we avoid doubly
      // reading in this mission OR ONE OF THIS MISSIONS VALUES!!!!
      // 
      // And we need of course to advance the array index for mission targets too...
      //
      MissionTargetPointer = EndOfMissionTargetPointer; // to avoid double entering the same target
      MissionTargetIndex++; // to avoid overwriting the same entry again

    } // while mission target found...


  //--------------------
  // Finally we record the number of mission targets scanned and are done with this function
  DebugPrintf ( 1 , "\nNUMBER OF MISSION TARGETS FOUND: %d.\n" , MissionTargetIndex );
  fflush( stdout );

}; // void Get_Mission_Targets( ... )

/* -----------------------------------------------------------------
 * This function initializes a completely new game within freedroid.
 * In contrast to InitFreedroid, this function should be called 
 * whenever or better before any new game is started.
 * -----------------------------------------------------------------*/
void
InitNewMissionList ( char *MissionName )
{
  char *fpath;
  int i , j ;
  char *MainMissionPointer;
  char *BriefingSectionPointer;
  char *EventSectionPointer;
  char *StartPointPointer;
  // char *MissionTargetPointer;
  char* Liftname;
  char* Crewname;
  char* GameDataName;
  char* Shipname;
  int NumberOfStartPoints=0;
  int RealStartPoint=0;
  int StartingLevel=0;
  int StartingXPos=0;
  int StartingYPos=0;
  int MissionTargetIndex = 0;
  int PlayerNum;

#define END_OF_MISSION_DATA_STRING "*** End of Mission File ***"
#define MISSION_BRIEFING_BEGIN_STRING "** Start of Mission Briefing Text Section **"
#define MISSION_ENDTITLE_SONG_NAME_STRING "Song name to play in the end title if the mission is completed: "
#define EVENT_SECTION_BEGIN_STRING "** Start of Mission Event Section **"
#define SHIPNAME_INDICATION_STRING "Ship file to use for this mission: "
#define ELEVATORNAME_INDICATION_STRING "Lift file to use for this mission: "
#define CREWNAME_INDICATION_STRING "Crew file to use for this mission: "
#define LIFTS_ON_INDICATION_STRING "Lifts On file to use for this mission: "
#define LIFTS_OFF_INDICATION_STRING "Lifts Off file to use for this mission: "
#define GAMEDATANAME_INDICATION_STRING "Physics ('game.dat') file to use for this mission: "
#define MISSION_ENDTITLE_BEGIN_STRING "** Beginning of End Title Text Section **"
#define MISSION_ENDTITLE_END_STRING "** End of End Title Text Section **"
#define MISSION_START_POINT_STRING "Possible Start Point : "

  // #define END_OF_MISSION_TARGET_STRING "*** End of Mission Target ***"
#define NEXT_MISSION_NAME_STRING "After completing this mission, load mission : "

  //--------------------
  // We store the mission name in case the influ
  // gets destroyed so we know where to continue in
  // case the player doesn't want to return to the very beginning
  // but just to replay this mission.
  //
  strcpy( Previous_Mission_Name , MissionName ); 
  
  DebugPrintf (2, "\nvoid InitNewMission( char *MissionName ): real function call confirmed...");
  DebugPrintf (1, "\nA new mission is being initialized from file %s.\n" , MissionName );

  //--------------------
  //At first we do the things that must be done for all
  //missions, regardless of mission file given
  Activate_Conservative_Frame_Computation();
  Total_Frames_Passed_In_Mission=0;
  LastBlastHit = 0;
  LastGotIntoBlastSound = 2;
  LastRefreshSound = 2;
  PlusExtentionsOn = FALSE;
  ThisMessageTime = 0;
  LevelDoorsNotMovedTime = 0.0;

  Me[0].Experience = 0; // This should be done at the end of the highscore list procedure
  ShowScore = 0; // This should be done at the end of the highscore list procedure

  //--------------------
  // Delete all bullets and blasts.  We need to do this AFTER the map has been
  // read in, since DeleteBullet calls StartBlast which accesses the map, which
  // should have been allocated at his point.
  //
  for (i = 0; i < MAXBULLETS; i++)
    {
      DeleteBullet ( i , FALSE );
    }
  DebugPrintf ( 1 , "\nvoid InitNewMission( ... ): All bullets have been deleted...");
  for (i = 0; i < MAXBLASTS; i++)
    {
      DeleteBlast( i );
    }
  DebugPrintf ( 1 , "\nvoid InitNewMission( ... ): All blasts have been deleted...");

  //--------------------
  //Now its time to start decoding the mission file.
  //For that, we must get it into memory first.
  //
  fpath = find_file (MissionName, MAP_DIR, FALSE);
  MainMissionPointer = ReadAndMallocAndTerminateFile( fpath , END_OF_MISSION_DATA_STRING ) ;

  //--------------------
  // Now the mission file is read into memory.  That means we can start to decode the details given
  // in the body of the mission file.  
  //

  // Now we search for the beginning of the WHOLE event section within the mission file
  EventSectionPointer = LocateStringInData ( MainMissionPointer , EVENT_SECTION_BEGIN_STRING );
  // Read in the events and triggers that can be used to cause and define something to happen
  Get_Game_Events ( EventSectionPointer );
  DebugPrintf (2, "\nvoid InitNewMission(void): Events and triggerable actions have been successfully read in...:");

  //--------------------
  // We start with doing the briefing things...
  // Now we search for the beginning of the mission briefing big section NOT subsection.
  // We display the title and explanation of controls and such... 
  BriefingSectionPointer = LocateStringInData ( MainMissionPointer , MISSION_BRIEFING_BEGIN_STRING );
  Title ( BriefingSectionPointer );
  DebugPrintf (2, "\nvoid InitNewMission(void): The title signaton has been successfully displayed...:");

  //--------------------
  // First we extract the game physics file name from the
  // mission file and load the game data.
  //
  GameDataName = 
    ReadAndMallocStringFromData ( MainMissionPointer , GAMEDATANAME_INDICATION_STRING , "\n" ) ;

  Init_Game_Data ( GameDataName );

  //--------------------
  // Now its time to get the shipname from the mission file and
  // read the ship file into the right memory structures
  //
  Shipname = 
    ReadAndMallocStringFromData ( MainMissionPointer , SHIPNAME_INDICATION_STRING , "\n" ) ;
  fpath = find_file (Shipname, MAP_DIR, FALSE);
  if ( LoadShip (fpath) == ERR )
    {
      DebugPrintf (1, "Error in LoadShip\n");
      Terminate (ERR);
    }

  //--------------------
  // Now its time to get the elevator file name from the mission file and
  // read the elevator file into the right memory structures
  //
  Liftname = 
    ReadAndMallocStringFromData ( MainMissionPointer , ELEVATORNAME_INDICATION_STRING , "\n" ) ;
  if (GetLiftConnections ( Liftname ) == ERR)
    {
      DebugPrintf (1, "\nError in GetLiftConnections ");
      Terminate (ERR);
    }

  //--------------------
  // Now its time to get the lifts on/off picturec file name from the mission file and
  // assemble an appropriate crew out of it
  //
  ship_on_filename = 
    ReadAndMallocStringFromData ( MainMissionPointer , LIFTS_ON_INDICATION_STRING , "\n" ) ;
  ship_off_filename = 
    ReadAndMallocStringFromData ( MainMissionPointer , LIFTS_OFF_INDICATION_STRING , "\n" ) ;

  //--------------------
  // We also load the comment for the influencer to say at the beginning of the mission
  //
  Me[0].TextToBeDisplayed =
    ReadAndMallocStringFromData ( MainMissionPointer , "Influs mission start comment=\"" , "\"" ) ;
  Me[0].TextVisibleTime = 0;


  //--------------------
  // Now its time to get the crew file name from the mission file and
  // assemble an appropriate crew out of it
  //
  Crewname =
    ReadAndMallocStringFromData ( MainMissionPointer , CREWNAME_INDICATION_STRING , "\n" ) ;
  /* initialize enemys according to crew file */
  // WARNING!! THIS REQUIRES THE freedroid.ruleset FILE TO BE READ ALREADY, BECAUSE
  // ROBOT SPECIFICATIONS ARE ALREADY REQUIRED HERE!!!!!
  if (GetCrew ( Crewname ) == ERR)
    {
      DebugPrintf (1, "\nInitNewGame(): ERROR: Initialization of enemys failed...");
      Terminate (-1);
    }

  //--------------------
  // Now its time to get the debriefing text from the mission file so that it
  // can be used, if the mission is completed and also the end title music name
  // must be read in as well
  //
  DebriefingSong = ReadAndMallocStringFromData ( MainMissionPointer , MISSION_ENDTITLE_SONG_NAME_STRING , "\n" ) ;
  DebriefingText =
    ReadAndMallocStringFromData ( MainMissionPointer , MISSION_ENDTITLE_BEGIN_STRING , MISSION_ENDTITLE_END_STRING ) ;

  //--------------------
  // Now we read all the possible starting points for the
  // current mission file, so that we know where to place the
  // influencer at the beginning of the mission.

  NumberOfStartPoints = CountStringOccurences ( MainMissionPointer , MISSION_START_POINT_STRING );

  if ( NumberOfStartPoints == 0 )
    {
      DebugPrintf ( 0 , "\n\nERROR! NOT EVEN ONE SINGLE STARTING POINT ENTRY FOUND!  TERMINATING!");
      Terminate( ERR );
    }
  DebugPrintf (1, "\nFound %d different starting points for the mission in the mission file.", NumberOfStartPoints );

  //--------------------
  // Now that we know how many different starting points there are, we can randomly select
  // one of them and read then in this one starting point into the right structures...
  //
  RealStartPoint = MyRandom ( NumberOfStartPoints -1 ) + 1;
  StartPointPointer=MainMissionPointer;
  for ( i=0 ; i<RealStartPoint; i++ )
    {
      StartPointPointer = strstr ( StartPointPointer , MISSION_START_POINT_STRING );
      StartPointPointer += strlen ( MISSION_START_POINT_STRING );
    }
  StartPointPointer = strstr( StartPointPointer , "Level=" ) + strlen( "Level=" );
  sscanf( StartPointPointer , "%d" , &StartingLevel );
  CurLevel = curShip.AllLevels[ StartingLevel ];
  Me[0].pos.z = StartingLevel;
  StartPointPointer = strstr( StartPointPointer , "XPos=" ) + strlen( "XPos=" );
  sscanf( StartPointPointer , "%d" , &StartingXPos );
  Me[0].pos.x=StartingXPos;
  StartPointPointer = strstr( StartPointPointer , "YPos=" ) + strlen( "YPos=" );
  sscanf( StartPointPointer , "%d" , &StartingYPos );
  Me[0].pos.y=StartingYPos;

  Me [ 0 ] . teleport_anchor . x = Me [ 0 ] . pos . x ;
  Me [ 0 ] . teleport_anchor . y = Me [ 0 ] . pos . y ;
  Me [ 0 ] . teleport_anchor . z = Me [ 0 ] . pos . z ;
  
  DebugPrintf ( 1 , "\nFinal starting position: Level=%d XPos=%d YPos=%d." , StartingLevel, StartingXPos, StartingYPos );
  
  //--------------------
  // At this point the position history can be initialized
  //
  InitInfluPositionHistory( 0 );

  //--------------------
  // Now we read in the mission targets for this mission
  // Several different targets may be specified simultaneously
  //
  Get_Mission_Targets( MainMissionPointer );

  //--------------------
  // After the mission targets have been successfully loaded now,
  // we need to add a pointer to the next mission, so that we will later
  // now which mission to load after this mission has been completed.
  //
  NextMissionName =
    ReadAndMallocStringFromData ( MainMissionPointer , NEXT_MISSION_NAME_STRING , "\n" ) ;

  // Reactivate the light on alle Levels, that might have been dark 
  for (i = 0; i < curShip.num_levels; i++)
    curShip.AllLevels[i]->empty = FALSE;
  DebugPrintf (2, "\nvoid InitNewMission( ... ): All levels have been set to 'active'...");

  // show the banner for the game
  ClearGraphMem();
  DisplayBanner (NULL, NULL,  BANNER_FORCE_UPDATE );
  InitBars = TRUE;

  SwitchBackgroundMusicTo ( CurLevel->Background_Song_Name );

  //--------------------
  // Now that the briefing and all that is done,
  // the influence structure can be initialized for
  // the new mission:
  Me[0].type = DRUID001;
  Me[0].speed.x = 0;
  Me[0].speed.y = 0;
  Me[0].autofire = FALSE;
  Me[0].status = MOBILE;
  Me[0].phase = 0;
  Me[0].MissionTimeElapsed=0;
  Me[0].Current_Victim_Resistance_Factor=1;
  Me[0].FramesOnThisLevel=0;
  Me[0].weapon_swing_time = (-1);  // currently not swinging this means...
  Me[0].got_hit_time = (-1);  // currently not stunned and needing time to recover...

  Me[0].points_to_distribute = 0;
  Me[0].ExpRequired = 1500;
  for ( i = 0 ; i < 1000 ; i ++ ) Me[0].KillRecord[ i ] = 0;
  for ( i = 0 ; i < MAX_LEVELS ; i ++ ) Me[0].HaveBeenToLevel [ i ] = FALSE ;
  for ( i = 0 ; i < MAX_ITEMS_PER_LEVEL ; i ++ ) 
    {
      Me[0].DetectedItemList[i].x = 0 ;
      Me[0].DetectedItemList[i].y = 0 ;
    }
  Me[0].exp_level = 1;
  Me[0].Gold = 100;

  Me[0].readied_skill = 0;
  for ( i = 0 ; i < NUMBER_OF_SKILLS ; i ++ ) 
    {
      Me[0].SkillLevel [ i ] = 0 ;
      Me[0].base_skill_level [ i ] = 0 ;
    }
  Me[0].SkillLevel [ 0 ] = 1 ;
  Me[0].base_skill_level [ 0 ] = 1;
  GameConfig.spell_level_visible = 0;

  Me [ 0 ] . melee_weapon_skill = 0 ;
  Me [ 0 ] . ranged_weapon_skill = 0 ;
  Me [ 0 ] . spellcasting_skill = 0 ;
  Me [ 0 ] . hacking_skill = 0 ;
  Me [ 0 ] . repair_skill = 0 ;

  UpdateAllCharacterStats( 0 );

  //--------------------
  // Now we disable all chat flags (i.e. the Tux hasn't spoken to
  // that person at all) for all the non-player-characters in the game
  //
  for ( i = 0 ; i < MAX_ANSWERS_PER_PERSON ; i ++ ) 
    {
      for ( j = 0 ; j < MAX_PERSONS ; j ++ )
	{
	  Me[0]. Chat_Flags [ j ] [ i ] = 0 ;
	  Me[0]. Chat_Flags [ j ] [ 0 ] = 1 ; // we always allow to say something in the beginning...
	}
    }

  //--------------------
  // At the beginning, the Tux shouldn't have any passwords
  // or security clearances on him, and that's was we ensure here.
  //
  for ( j = 0 ; j < MAX_CLEARANCES ; j ++ )
    {
      Me [ 0 ] . clearance_list [ j ] = 0 ;
    }
  Me [ 0 ] . clearance_list [ 0 ] = 4 ;
  Me [ 0 ] . clearance_list [ 1 ] = 5 ;
  Me [ 0 ] . clearance_list [ 2 ] = 6 ;
  for ( j = 0 ; j < MAX_PASSWORDS ; j ++ )
    {
      strcpy ( Me [ 0 ] . password_list [ j ] , "" ) ;
    }
  strcpy ( Me [ 0 ] . password_list [ 0 ] , "Tux Himself" ) ;
  strcpy ( Me [ 0 ] . password_list [ 1 ] , "Tux Dummy1" ) ;
  strcpy ( Me [ 0 ] . password_list [ 2 ] , "Tux Dummy2" ) ;
  strcpy ( Me [ 0 ] . password_list [ 3 ] , "Tux Energy" ) ;

  //--------------------
  // When the Tux arrives, he also should be at perfect health
  // and also full with all the mana he can have on him.
  //
  Me[0].energy = Me[0].maxenergy;
  Me[0].mana = Me[0].maxmana;
  DebugPrintf( 1 , "\n Me[0].energy : %f . " , Me[0].energy );
  Me[0].health = Me[0].energy;	/* start with max. health */

  //--------------------
  // None of the inventory slots like currently equipped weapons
  // or the like should be held in hand, like when you take it
  // 'into your hand' by clicking on it with the mouse button in
  // the inventory screen.
  //
  Me[0].weapon_item.currently_held_in_hand = FALSE;
  Me[0].armour_item.currently_held_in_hand = FALSE;
  Me[0].shield_item.currently_held_in_hand = FALSE;
  Me[0].special_item.currently_held_in_hand = FALSE;
  Me[0].drive_item.currently_held_in_hand = FALSE;
  Me[0].aux1_item.currently_held_in_hand = FALSE;
  Me[0].aux2_item.currently_held_in_hand = FALSE;

  FillInItemProperties ( & ( Me[0].weapon_item ) , TRUE , 0 );
  FillInItemProperties ( & ( Me[0].drive_item ) , TRUE , 0 );


  Item_Held_In_Hand = ( -1 );

  ShuffleEnemys( Me[0].pos.z ); // NOTE: THIS REQUIRES CurLevel TO BE INITIALIZED !! --> NOT ANY MORE!!!

  //--------------------
  // Now we start those missions, that are to be assigned automatically to the
  // player at game start
  //
  for ( MissionTargetIndex = 0 ; MissionTargetIndex < MAX_MISSIONS_IN_GAME ; MissionTargetIndex ++ )
    {
      if ( Me[0].AllMissions[ MissionTargetIndex ].AutomaticallyAssignThisMissionAtGameStart ) 
	{
	  AssignMission( MissionTargetIndex );
	}
    }

  //--------------------
  // To initialize the other players as well, for now, we make
  // a copy of the main players struct right over the other players
  // structs...
  //
  for ( PlayerNum = 1 ; PlayerNum < MAX_PLAYERS ; PlayerNum ++ )
    {
      // memcpy ( & ( Me [ PlayerNum ] ) , & ( Me [ 0 ] ) , sizeof ( Me [ 0 ] ) );
      Me [ PlayerNum ] . status = OUT ;
    }

}; // void InitNewMissionList ( char* MissionName )

/* ----------------------------------------------------------------------
 * This function clears out the Automap data.
 * ---------------------------------------------------------------------- */
void 
ClearAutomapData( void )
{
  int x , y , level ;

  for ( level = 0 ; level < MAX_LEVELS ; level ++ )
    {
      for ( y = 0 ; y < 200 ; y ++ )
	{
	  for ( x = 0 ; x < 200 ; x ++ )
	    {
	      Me [ 0 ] . Automap[level][y][x] = 0 ;
	    }
	}
    }

}; // void ClearAutomapData ( void )

/* ----------------------------------------------------------------------
 * This function sets the GameConfig back to the default values, NOT THE
 * VALUES STORED IN THE USERS CONFIG FILE.  This function is useful if 
 * no config file if found or if the config file turns out to originate
 * from a different version of freedroid, which could be dangerous as
 * well.
 * ---------------------------------------------------------------------- */
void 
Reset_GameConfig_To_Default_Values ( void )
{
  //--------------------
  // At first we set audio volume to maximum value.
  // This might be replaced later with values from a 
  // private user Freedroid config file.  But for now
  // this code is good enough...
  //
  GameConfig.Current_BG_Music_Volume=1.0;
  GameConfig.Current_Sound_FX_Volume=0.5;

  GameConfig.WantedTextVisibleTime = 3;
  GameConfig.Draw_Framerate=FALSE;
  GameConfig.All_Texts_Switch=TRUE;
  GameConfig.Enemy_Hit_Text=FALSE;
  GameConfig.Enemy_Bump_Text=TRUE;
  GameConfig.Enemy_Aim_Text=TRUE;
  GameConfig.Influencer_Refresh_Text=FALSE;
  GameConfig.Influencer_Blast_Text=TRUE;
  GameConfig.Mission_Log_Visible_Max_Time = 5;
  GameConfig.Inventory_Visible_Max_Time = 5;
  // GameConfig.Theme_SubPath="lanzz_theme/";
  strcpy ( GameConfig.Theme_SubPath , "lanzz_theme/" );
  GameConfig.Draw_Framerate=TRUE;
  GameConfig.Draw_Energy=FALSE;
  GameConfig.Draw_Position=FALSE;
  GameConfig.All_Texts_Switch = FALSE;

  GameConfig.terminate_on_missing_speech_sample = FALSE ;
  GameConfig.show_subtitles_in_dialogs = TRUE ;
  GameConfig.show_digits_of_droids = FALSE ;

}; // void Reset_GameConfig_To_Default_Values ( void )

/* -----------------------------------------------------------------
 * This function initializes the whole Freedroid game.
 * 
 * THIS MUST NOT BE CONFUSED WITH INITNEWGAME, WHICH
 * ONLY INITIALIZES A NEW MISSION FOR THE GAME.
 *  
 * ----------------------------------------------------------------- */
void
InitFreedroid ( void )
{
  struct timeval timestamp;
  int i;

  // InvincibleMode = TRUE;

  //--------------------
  // It might happen, that the uninitialized AllBullets array contains a 1
  // somewhere and that the bullet is deleted and the surface freed, where
  // it never has been allocated, resulting in a SEGFAULT.  This has never
  // happend, but for security, we add this loop to clean out these important 
  // flags.       It should be sufficient to do this here, since the flag
  // will never be set again if not Surfaces are allocated too and then they
  // can of course also be freed as well.

  Bulletmap=NULL;  // That will cause the memory to be allocated later
  for ( i = 0 ; i < MAXBULLETS ; i++ )
    {
      AllBullets[i].Surfaces_were_generated = FALSE;
      DeleteBullet( i , FALSE );
    }

  ServerMode = FALSE;
  ClientMode = FALSE;

  Overall_Average=0.041;
  SkipAFewFrames = 0;
  Me[0].TextVisibleTime = 0;
  Me[0].readied_skill = 0;
  CurLevel = NULL;  // please leave this here.  It indicates, that the map is not yet initialized!!!
  // Me[0].TextToBeDisplayed = "Hello, I'm 001.";
  Me[0].TextToBeDisplayed = "Linux Kernel booted.  001 transfer-tech modules loaded.  System up and running.";

  // --------------------
  //
  InventorySize.x = 9;
  InventorySize.y = 6;

  Reset_GameConfig_To_Default_Values ();

  //Load user config file if it exists...
  LoadGameConfig ();

  Copy_Rect (Full_User_Rect, User_Rect);

  InitVideo ();

  ShowStartupPercentage ( 2 ) ; 

  InitAudio ();

  ShowStartupPercentage ( 4 ) ; 
  
  Init_Joy ();

  //--------------------
  // Now we prepare the automap data for later use
  //
  GameConfig.Automap_Visible = TRUE;
  ClearAutomapData( );

  ShowStartupPercentage ( 6 ) ; 

  Init_Network ();

  ShowStartupPercentage ( 8 ) ; 

  Init_Game_Data("freedroid.ruleset");  // load the default ruleset. This can be
			       // overwritten from the mission file.

  ShowStartupPercentage ( 10 ) ; 

  // The default should be, that no rescaling of the
  // combat window at all is done.
  CurrentCombatScaleFactor = 1;

  /* 
   * Initialise random-number generator in order to make 
   * level-start etc really different at each program start
   */
  gettimeofday(&timestamp, NULL);
  srand((unsigned int) timestamp.tv_sec); /* yes, we convert long->int here! */

  // initialize the highscore list 
  Init_Highscores ();
 
  HideInvisibleMap = FALSE;	/* Hide invisible map-parts. Para-extension!! */

  MinMessageTime = 55;
  MaxMessageTime = 850;

  CurLevel = NULL; // please leave this here BEFORE InitPictures

  /* Now fill the pictures correctly to the structs */
  if (!InitPictures ())
    {		
      DebugPrintf (1, "\n Error in InitPictures reported back...\n");
      Terminate(ERR);
    }

  ShowStartupPercentage ( 99 ) ; 

}; // void InitFreedroid ( void ) 

/* -----------------------------------------------------------------
 * This function does the mission briefing.  It assumes, 
 * that a mission file has already been successfully loaded into
 * memory.  The briefing texts will be extracted and displayed in
 * scrolling font.
 * -----------------------------------------------------------------*/
void
Title ( char *MissionBriefingPointer )
{
  int ScrollEndLine = User_Rect.y;	// endpoint for scrolling...
  char* NextSubsectionStartPointer;
  char* PreparedBriefingText;
  char* TerminationPointer;
  char* TitlePictureName;
  char* TitleSongName;

  int ThisTextLength;
#define BRIEFING_TITLE_PICTURE_STRING "The title picture in the graphics subdirectory for this mission is : "
#define BRIEFING_TITLE_SONG_STRING "The title song in the sound subdirectory for this mission is : "
#define NEXT_BRIEFING_SUBSECTION_START_STRING "* New Mission Briefing Text Subsection *"
#define END_OF_BRIEFING_SUBSECTION_STRING "* End of Mission Briefing Text Subsection *"

  // STRANGE!! This command will be silently ignored by SDL?
  // WHY?? DONT KNOW!!!
  // PlaySound ( CLASSICAL_BEEP_BEEP_BACKGROUND_MUSIC );
  // PlaySound ( CLASSICAL_BEEP_BEEP_BACKGROUND_MUSIC );
  // SwitchBackgroundMusicTo ( COMBAT_BACKGROUND_MUSIC_SOUND );

  TitleSongName = ReadAndMallocStringFromData ( MissionBriefingPointer, BRIEFING_TITLE_SONG_STRING , "\n" ) ;

  SwitchBackgroundMusicTo ( TitleSongName );

  TitlePictureName = ReadAndMallocStringFromData ( MissionBriefingPointer, BRIEFING_TITLE_PICTURE_STRING , "\n" ) ;

  SDL_SetClipRect ( Screen, NULL );
  // DisplayImage ( find_file(TitlePictureName, GRAPHICS_DIR, FALSE) );
  // SDL_Flip (Screen);

  Me[0].status=BRIEFING;

  // ClearGraphMem ();
  // DisplayBanner (NULL, NULL,  BANNER_FORCE_UPDATE ); 

  // SetCurrentFont( FPS_Display_BFont );
  SetCurrentFont( Para_BFont );


  // Next we display all the subsections of the briefing section
  // with scrolling font
  NextSubsectionStartPointer = MissionBriefingPointer;
  while ( ( NextSubsectionStartPointer = strstr ( NextSubsectionStartPointer, NEXT_BRIEFING_SUBSECTION_START_STRING)) != NULL)
    {
      NextSubsectionStartPointer += strlen ( NEXT_BRIEFING_SUBSECTION_START_STRING );
      if ( (TerminationPointer=strstr ( NextSubsectionStartPointer, END_OF_BRIEFING_SUBSECTION_STRING)) == NULL)
	{
	  DebugPrintf (1, "\n\nvoid Title(...): Unterminated Subsection in Mission briefing....Terminating...");
	  Terminate(ERR);
	}
      ThisTextLength=TerminationPointer-NextSubsectionStartPointer;
      PreparedBriefingText = MyMalloc (ThisTextLength + 10);
      strncpy ( PreparedBriefingText , NextSubsectionStartPointer , ThisTextLength );
      PreparedBriefingText[ThisTextLength]=0;
      
      // DebugPrintf (1, "\n\nIdentified Text for the scrolling briefing: %s." , PreparedBriefingText);
      fflush(stdout);
      
      ScrollText ( PreparedBriefingText, SCROLLSTARTX, SCROLLSTARTY, ScrollEndLine , TitlePictureName );
      free ( PreparedBriefingText );
    }

  ClearGraphMem ();
  DisplayBanner (NULL, NULL,  BANNER_FORCE_UPDATE ); 
  SDL_Flip( Screen );

  return;

}; // void Title ( void )

/* ----------------------------------------------------------------------
 * This function is for the end of the game in case of the player having
 * won.
 * ---------------------------------------------------------------------- */
void
EndTitle (void)
{
  int ScrollEndLine = User_Rect.y;	// endpoint for scrolling...

  DebugPrintf (2, "\nvoid EndTitle(void): real function call confirmed...:");

  SwitchBackgroundMusicTo ( DebriefingSong );

  DisplayBanner (NULL, NULL,  BANNER_FORCE_UPDATE );

  // SetCurrentFont( FPS_Display_BFont );
  SetCurrentFont( Para_BFont );

  ScrollText ( DebriefingText , SCROLLSTARTX, SCROLLSTARTY, ScrollEndLine , NE_TITLE_PIC_FILE );

  while ( SpacePressed() );

}; // void EndTitle( void ) 

/* ----------------------------------------------------------------------
 * This function displayes the last seconds of the game when the influencer
 * has actually been killed.  It generates some explosions and waits for
 * some seconds, where the user can reload his latest game, or after that
 * returns to finally quit the inner game loop and the program will 
 * (outside this function) ask for a completely new game or loading a different
 * saved game or quit as in the very beginning of the game.
 * ---------------------------------------------------------------------- */
void
ThouArtDefeated (void)
{
  int j;
  int now;

  DebugPrintf (2, "\nvoid ThouArtDefeated(void): Real function call confirmed.");
  Me[0].status = TERMINATED;
  GameConfig.Inventory_Visible=FALSE;
  GameConfig.CharacterScreen_Visible=FALSE;
  GameConfig.Mission_Log_Visible=FALSE;
  ThouArtDefeatedSound ();
  ExplodeInfluencer ();
  now=SDL_GetTicks();

  //--------------------
  // Now that the influencer is dead, all this precious items
  // spring off of him...
  //
  DropItemAt ( Me[0].weapon_item.type , Me[0].pos.x - 0.5 , Me[0].pos.y - 0.5 , -1 , -1 , 0 );
  DropItemAt ( Me[0].drive_item.type  , Me[0].pos.x + 0.5 , Me[0].pos.y - 0.5 , -1 , -1 , 0 );
  DropItemAt ( Me[0].shield_item.type , Me[0].pos.x + 0.5 , Me[0].pos.y + 0.5 , -1 , -1 , 0 );
  DropItemAt ( Me[0].armour_item.type , Me[0].pos.x - 0.5 , Me[0].pos.y + 0.5 , -1 , -1 , 0 );
  DropItemAt ( Me[0].special_item.type , Me[0].pos.x - 0.5 , Me[0].pos.y , -1 , -1 , 0 );
  DropItemAt ( Me[0].aux1_item.type , Me[0].pos.x + 0.5 , Me[0].pos.y , -1 , -1 , 0 );
  DropItemAt ( Me[0].aux2_item.type , Me[0].pos.x , Me[0].pos.y - 0.5 , -1 , -1 , 0 );
  DropItemAt ( ITEM_MONEY , Me[0].pos.x , Me[0].pos.y , -1 , -1 , 0 );

  GameOver = TRUE;

  while ( ( SDL_GetTicks() - now < 1000 * WAIT_AFTER_KILLED ) && ( GameOver == TRUE ) )
    {
      StartTakingTimeForFPSCalculation(); 

      Assemble_Combat_Picture ( DO_SCREEN_UPDATE );
      DisplayBanner (NULL, NULL,  0 );
      ExplodeBlasts ();
      MoveBullets ();
      MoveEnemys ();
      for ( j = 0 ; j < MAX_PLAYERS ; j ++ ) MoveLevelDoors ( j );	

      ReactToSpecialKeys();

      for (j = 0; j < MAXBULLETS; j++)
	CheckBulletCollisions (j);

      ComputeFPSForThisFrame();

    }

  // update_highscores ();
  // EscapeMenu();

  DebugPrintf (2, "\nvoid ThouArtDefeated(void): Usual end of function reached.");
  DebugPrintf (1, "\n\n DefeatedDone \n\n");
}; // void ThouArtDefeated(void)

/* ----------------------------------------------------------------------
 * This is an old function, that was used in the classical paradroid in
 * order to detect a completely cleared ship, but only after some seconds.
 * Maybe that could be removed from the code already, since we have the
 * CheckIfMissionIsComplete function.
 * ---------------------------------------------------------------------- */
void
ThouArtVictorious (void)
{
  ShipEmptyCounter = WAIT_SHIPEMPTY;
  GameOver = TRUE;

  ClearUserFenster ();
  getchar_raw ();
}

/*----------------------------------------------------------------------
 * This function checks, if the influencer has succeeded in his given 
 * mission.  If not it returns, if yes the EndTitle/Debriefing is
 * started.
 ----------------------------------------------------------------------*/
void 
CheckIfMissionIsComplete (void)
{
  int Robot_Counter;
  int ItemCounter;
  int MissNum;
  int ActionNum;
  static int CheckMissionGrid; 
  
  //--------------------
  // We do not need to check for mission completed EVERY frame
  // It will be enough to do it now and then..., e.g. every 50th frame
  //
  CheckMissionGrid++;
  if ( ( CheckMissionGrid % 50 ) != 0 ) return;

  //--------------------
  // We set up a cheat code, so that one can easily 'complete' a mission
  //
  if ( MPressed() && Alt_Was_Pressed()
       && Ctrl_Was_Pressed() && Shift_Was_Pressed() )
    {
      EndTitle();
      InitNewMissionList ( NextMissionName );
    }
#define MIS_COMPLETE_DEBUG 0


  for ( MissNum = 0 ; MissNum < MAX_MISSIONS_IN_GAME ; MissNum ++ )
    {

      //--------------------
      // We need not do anything, if the mission has already failed or if
      // the mission is already completed or if the mission does not exist
      // at all or if the mission was not assigned yet
      //
      if ( Me[0].AllMissions[ MissNum ].MissionIsComplete == TRUE ) continue;
      if ( Me[0].AllMissions[ MissNum ].MissionWasFailed == TRUE ) continue;
      if ( Me[0].AllMissions[ MissNum ].MissionExistsAtAll != TRUE ) continue;
      if ( Me[0].AllMissions[ MissNum ].MissionWasAssigned != TRUE ) continue;

      DebugPrintf ( 0 , "\nSomething was assigned at all..... MissNum = %d " , MissNum );

      //--------------------
      // Continue if the Mission target KillOne is given but not fullfilled
      //
      if ( Me[0].AllMissions[ MissNum ].KillOne != (-1) )
	{
	  //	  for ( Robot_Counter=0 ; Robot_Counter < MAX_ENEMYS_ON_SHIP ; Robot_Counter++ )
	  for ( Robot_Counter=0 ; Robot_Counter < Number_Of_Droids_On_Ship ; Robot_Counter++ )
	    {
	      if ( ( AllEnemys[Robot_Counter].energy > 0 ) && 
		   ( AllEnemys[Robot_Counter].Status != OUT ) && 
		   ( AllEnemys[Robot_Counter].Marker == Me[0].AllMissions[ MissNum ].KillOne ) )
		{
		  DebugPrintf ( 0 , "\nOne of the marked droids is still alive...");
		  goto CheckNextMission;
		}
	    }
	}

      //--------------------
      // Continue if the Mission target fetch_item is given but not fullfilled
      //
      if ( Me[0].AllMissions[ MissNum ].fetch_item != (-1) )
	{

	  for ( ItemCounter = 0 ; ItemCounter < MAX_ITEMS_IN_INVENTORY ; ItemCounter++ )
	    {
	      if ( Me [ 0 ] . Inventory [ ItemCounter ] . type == Me[0].AllMissions[ MissNum ].fetch_item )
		{
		  DebugPrintf ( MIS_COMPLETE_DEBUG , "\nDesired item IS PRESENT!!");
		  break;
		}
	    }
	  if ( ItemCounter >= MAX_ITEMS_IN_INVENTORY ) goto CheckNextMission;
	}

      //--------------------
      // Continue if the Mission target KillAll is given but not fullfilled
      //
      if ( Me[0].AllMissions[ MissNum ].KillAll != (-1) )
	{
	  //	  for ( Robot_Counter=0 ; Robot_Counter < MAX_ENEMYS_ON_SHIP ; Robot_Counter++ )
	  for ( Robot_Counter=0 ; Robot_Counter < Number_Of_Droids_On_Ship ; Robot_Counter++ )
	    {
	      if ( ( AllEnemys[Robot_Counter].energy > 0 ) && ( AllEnemys[Robot_Counter].is_friendly == FALSE ) )
		{
		  DebugPrintf ( MIS_COMPLETE_DEBUG , "\nThere are some robots still alive, and you should kill them all...");
		  fflush(stdout);
		  goto CheckNextMission;
		}
	    }
	}

      //--------------------
      // Continue if the Mission target KillClass is given but not fullfilled
      //
      if ( Me[0].AllMissions[ MissNum ].KillClass != (-1) )
	{
	  // for ( Robot_Counter=0 ; Robot_Counter < MAX_ENEMYS_ON_SHIP ; Robot_Counter++ )
	  for ( Robot_Counter=0 ; Robot_Counter < Number_Of_Droids_On_Ship ; Robot_Counter++ )
	    {
	      if ( ( AllEnemys[Robot_Counter].energy > 0 ) && 
		   ( AllEnemys[Robot_Counter].Status != OUT ) && 
		   ( Druidmap[AllEnemys[Robot_Counter].type].class == Me[0].AllMissions[ MissNum ].KillClass ) ) 
		{
		  DebugPrintf ( MIS_COMPLETE_DEBUG , "\nOne of that class is still alive: Nr=%d Lev=%d X=%f Y=%f." , 
				Robot_Counter , AllEnemys[Robot_Counter].pos.z , 
				AllEnemys[Robot_Counter].pos.x , AllEnemys[Robot_Counter].pos.y );
		  goto CheckNextMission;
		}
	    }
	}
      
      //--------------------
      // Continue if the Mission target MustBeClass is given but not fullfilled
      //
      if ( Me[0].AllMissions[ MissNum ].MustBeClass != (-1) )
	{
	  DebugPrintf ( MIS_COMPLETE_DEBUG , "\nMe[0].type is now: %d.", Me[0].type );
	  if ( Druidmap[Me[0].type].class != Me[0].AllMissions[ MissNum ].MustBeClass ) 
	    {
	      DebugPrintf ( MIS_COMPLETE_DEBUG , "\nMe[0].class does not match...");
	      continue;
	    }
	}
      

      //--------------------
      // Continue if the Mission target MustBeType is given but not fullfilled
      //
      if ( Me[0].AllMissions[ MissNum ].MustBeType != (-1) )
	{
	  DebugPrintf ( MIS_COMPLETE_DEBUG , "\nMe[0].type is now: %d.", Me[0].type );
	  if ( Me[0].type != Me[0].AllMissions[ MissNum ].MustBeType ) 
	    {
	      DebugPrintf ( MIS_COMPLETE_DEBUG , "\nMe[0].type does not match...");
	      continue;
	    }
	}
      

      //--------------------
      // Continue if the Mission target MustReachLevel is given but not fullfilled
      //
      if ( Me[0].AllMissions[ MissNum ].MustReachLevel != (-1) )
	{
	  if ( CurLevel->levelnum != Me[0].AllMissions[ MissNum ].MustReachLevel ) 
	    {
	      DebugPrintf ( MIS_COMPLETE_DEBUG , "\nLevel number does not match...");
	      continue;
	    }
	}
      
      //--------------------
      // Continue if the Mission target MustReachPoint.x is given but not fullfilled
      //
      if ( Me[0].AllMissions[ MissNum ].MustReachPoint.x != (-1) )
	{
	  if ( Me[0].pos.x != Me[0].AllMissions[ MissNum ].MustReachPoint.x ) 
	    {
	      DebugPrintf ( MIS_COMPLETE_DEBUG , "\nX coordinate does not match...");
	      continue;
	    }
	}
      
      //--------------------
      // Continue if the Mission target MustReachPoint.y is given but not fullfilled
      //
      if ( Me[0].AllMissions[ MissNum ].MustReachPoint.y != (-1) )
	{
	  if ( Me[0].pos.y != Me[0].AllMissions[ MissNum ].MustReachPoint.y ) 
	    {
	      DebugPrintf ( MIS_COMPLETE_DEBUG , "\nY coordinate does not match..."); 
	      continue;
	    }
	}
      
      //--------------------
      // Continue if the Mission target MustLiveTime is given but not fullfilled
      //
      if ( Me[0].AllMissions[ MissNum ].MustLiveTime != (-1) )
	{
	  if ( Me[0].MissionTimeElapsed < Me[0].AllMissions[ MissNum ].MustLiveTime ) 
	    {
	      DebugPrintf ( MIS_COMPLETE_DEBUG , "\nTime Limit not yet reached...");
	      continue;
	    }
	}
      
      //--------------------
      // Continue if the Mission target MustBeOne is given but not fullfilled
      //
      if ( Me[0].AllMissions[ MissNum ].MustBeOne != (-1) )
	{
	  if ( Me[0].Marker != Me[0].AllMissions[ MissNum ].MustBeOne ) 
	    {
	      DebugPrintf ( MIS_COMPLETE_DEBUG , "\nYou're not yet one of the marked ones...");
	      continue;
	    }
	}

      //--------------------
      // AT THIS POINT WE KNOW THAT ALL OF THE GIVEN TARGETS FOR THIS MISSION ARE FULLFILLED
      // We therefore mark the mission as completed
      //
      GameConfig.Mission_Log_Visible_Time = 0;
      GameConfig.Mission_Log_Visible = TRUE;
      Me[0].AllMissions[ MissNum ].MissionIsComplete = TRUE;
      Mission_Status_Change_Sound ( );
      for ( ActionNum = 0 ; ActionNum < MAX_MISSION_TRIGGERED_ACTIONS ; ActionNum ++ )
	{
	  ExecuteEvent( Me[0].AllMissions[ MissNum ].ListOfActionsToBeTriggeredAtCompletition[ ActionNum ] , 0 );
	}
      
    CheckNextMission: // this is a label for goto jumps.  Please don't remove it.
    } // for AllMissions
      
}; // void CheckIfMissionIsComplete

#undef _init_c



