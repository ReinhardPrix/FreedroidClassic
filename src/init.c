/*----------------------------------------------------------------------
 *
 * Desc: the paraplus initialisation routines
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

#define MISSION_COMPLETE_BONUS 1000

/*@Function============================================================
@Desc: This function loads all the constant variables of the game from
       a dat file, that should be optimally human readable.

@Ret: 
* $Function----------------------------------------------------------*/
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
 * 
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

  DebugPrintf (2, "\n\nStarting to read bullet data...\n\n");
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
      Bulletmap = MyMalloc ( i * (Number_Of_Bullet_Types + 1) + 1 );
      DebugPrintf (1, "\nWe have counted %d different bullet types in the game data file." , Number_Of_Bullet_Types );
      DebugPrintf (1, "\nMEMORY HAS BEEN ALLOCATED.\nTHE READING CAN BEGIN.\n" );
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

      // Now we read in the recharging time for this bullettype(=weapontype)
      ReadValueFromString( BulletPointer ,  BULLET_RECHARGE_TIME_BEGIN_STRING , "%lf" , 
			   &Bulletmap[BulletIndex].recharging_time , EndOfBulletData );

      // Now we read in the maximal speed this type of bullet can go.
      ReadValueFromString( BulletPointer ,  BULLET_SPEED_BEGIN_STRING , "%lf" , 
			   &Bulletmap[BulletIndex].speed , EndOfBulletData );

      // Now we read in the damage this bullet can do
      ReadValueFromString( BulletPointer ,  BULLET_DAMAGE_BEGIN_STRING , "%d" , 
			   &Bulletmap[BulletIndex].damage , EndOfBulletData );

      // Now we read in if you can fire before the previous bullet has expired
      ReadValueFromString( BulletPointer ,  BULLET_ONE_SHOT_ONLY_AT_A_TIME , "%d" , 
			   &Bulletmap[BulletIndex].oneshotonly , EndOfBulletData );

      // Now we read in the number of phases that are designed for this bullet type
      // THIS IS NOW SPECIFIED IN THE THEME CONFIG FILE
      // ReadValueFromString( BulletPointer ,  BULLET_NUMBER_OF_PHASES_BEGIN_STRING , "%d" , 
      // &Bulletmap[BulletIndex].phases , EndOfBulletData );

      // Now we read in the type of blast this bullet will cause when crashing e.g. against the wall
      ReadValueFromString( BulletPointer ,  BULLET_BLAST_TYPE_CAUSED_BEGIN_STRING , "%d" , 
			   &Bulletmap[BulletIndex].blast , EndOfBulletData );
 
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
      Bulletmap[i].speed *= bullet_speed_calibrator;
      Bulletmap[i].damage *= bullet_damage_calibrator;
    }

  DebugPrintf (1, "\nEnd of Get_Bullet_Data ( char* DataPointer ) reached.");
} // void Get_Bullet_Data ( char* DataPointer );


/*
----------------------------------------------------------------------

----------------------------------------------------------------------
*/

void 
Get_Mission_Events ( char* EventSectionPointer )
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

#define EVENT_ACTION_MAPCHANGE_POS_X_STRING "Action is mapchange at positionX="
#define EVENT_ACTION_MAPCHANGE_POS_Y_STRING "Action is mapchange at positionY="
#define EVENT_ACTION_MAPCHANGE_MAPLEVEL_STRING "Action is mapchange at maplevel="
#define EVENT_ACTION_MAPCHANGE_TO_WHAT_STRING "Action is change map there to new value="
#define EVENT_ACTION_INFLUENCER_SAY_SOMETHING "Action is Influencer saying something="
#define EVENT_ACTION_INFLUENCER_SAY_TEXT "Action is Influencer saying the following text=\""
#define EVENT_ACTION_INDEX_NUMBER_TO_USE_STRING "ACTION INDEX NUMBER TO USE="

#define EVENT_TRIGGER_POS_X_STRING "Influencer must be at x-coordinate="
#define EVENT_TRIGGER_POS_Y_STRING "Influencer must be at y-coordinate="
#define EVENT_TRIGGER_POS_MAPLEVEL_STRING "Influencer must be at maplevel="
#define EVENT_TRIGGER_WHICH_ACTION_STRING "Event Action to be triggered by this trigger="
#define EVENT_TRIGGER_DELETED_AFTER_TRIGGERING "Delete the event trigger after it has been triggered="

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
      AllEventTriggers[i].EventNumber=-1;
    }
  for ( i = 0 ; i < MAX_TRIGGERED_ACTIONS ; i++ )
    {
      // Maybe the triggered event consists of the influencer saying something
      AllTriggeredActions[i].InfluencerSaySomething=-1;
      AllTriggeredActions[i].InfluencerSayText="";
      // Maybe the triggered event consists of the map beeing changed at some tile
      AllTriggeredActions[i].ChangeMap=-1;
      AllTriggeredActions[i].ChangeMapLevel=-1;
      AllTriggeredActions[i].ChangeMapLocation.x=-1;
      AllTriggeredActions[i].ChangeMapLocation.y=-1;
      AllTriggeredActions[i].ChangeMapTo=-1;
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
      ReadValueFromString( EventPointer , EVENT_ACTION_INDEX_NUMBER_TO_USE_STRING , "%d" , 
			   &EventActionNumber , EndOfEvent );

      // Now we read in the map changing position in x and y coordinates
      ReadValueFromString( EventPointer , EVENT_ACTION_MAPCHANGE_POS_X_STRING , "%d" , 
			   &AllTriggeredActions[ EventActionNumber ].ChangeMapLocation.x , EndOfEvent );
      ReadValueFromString( EventPointer , EVENT_ACTION_MAPCHANGE_POS_Y_STRING , "%d" , 
			   &AllTriggeredActions[ EventActionNumber ].ChangeMapLocation.y , EndOfEvent );

      // Now we read in the map changing position level
      ReadValueFromString( EventPointer , EVENT_ACTION_MAPCHANGE_MAPLEVEL_STRING , "%d" , 
			   &AllTriggeredActions[ EventActionNumber ].ChangeMapLevel , EndOfEvent );

      // Now we read in the new value for that map tile
      ReadValueFromString( EventPointer , EVENT_ACTION_MAPCHANGE_TO_WHAT_STRING , "%d" , 
			   &AllTriggeredActions[ EventActionNumber ].ChangeMapTo , EndOfEvent );

      // Now we read in if the influencer is to say something
      ReadValueFromString( EventPointer , EVENT_ACTION_INFLUENCER_SAY_SOMETHING , "%d" , 
			   &AllTriggeredActions[ EventActionNumber ].InfluencerSaySomething , EndOfEvent );

      // Now we read in if the text for the influencer to say
      AllTriggeredActions[ EventActionNumber].InfluencerSayText =
	ReadAndMallocStringFromData ( EventPointer , EVENT_ACTION_INFLUENCER_SAY_TEXT , "\"" ) ;

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
      ReadValueFromString( EventPointer , EVENT_TRIGGER_WHICH_ACTION_STRING , "%d" , 
			   &AllEventTriggers[ EventTriggerNumber ].EventNumber , EndOfEvent );

      EventTriggerNumber++;
    } // While Event trigger begin string found...


  DebugPrintf (1 , "\nThat must have been the last Event Trigger section.");

} // void Get_Mission_Events ( char* EventSectionPointer );


/*@Function============================================================
@Desc: This function loads all the constant variables of the game from
       a dat file, that should be optimally human readable.

@Ret: 
* $Function----------------------------------------------------------*/
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
#define MAXSPEED_BEGIN_STRING "Maximum speed of this droid: "
#define CLASS_BEGIN_STRING "Class of this droid: "
#define ACCELERATION_BEGIN_STRING "Maximum acceleration of this droid: "
#define MAXENERGY_BEGIN_STRING "Maximum energy of this droid: "
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
#define ADVANCED_FIGHTING_BEGIN_STRING "Advanced Fighting present in this droid : "
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

      // Now we read in the maximal speed this droid can go. 
      ReadValueFromString( RobotPointer , MAXSPEED_BEGIN_STRING , "%lf" , 
			   &Druidmap[RobotIndex].maxspeed , EndOfDataPointer );

      // Now we read in the class of this droid.
      ReadValueFromString( RobotPointer , CLASS_BEGIN_STRING , "%d" , 
			   &Druidmap[RobotIndex].class , EndOfDataPointer );

      // Now we read in the maximal acceleration this droid can go. 
      ReadValueFromString( RobotPointer , ACCELERATION_BEGIN_STRING , "%lf" , 
			   &Druidmap[RobotIndex].accel , EndOfDataPointer );

      // Now we read in the maximal energy this droid can store. 
      ReadValueFromString( RobotPointer , MAXENERGY_BEGIN_STRING , "%lf" , 
			   &Druidmap[RobotIndex].maxenergy , EndOfDataPointer );

      // Now we read in the lose_health rate.
      ReadValueFromString( RobotPointer , LOSEHEALTH_BEGIN_STRING , "%lf" , 
			   &Druidmap[RobotIndex].lose_health , EndOfDataPointer );

      // Now we read in the class of this droid.
      ReadValueFromString( RobotPointer , GUN_BEGIN_STRING , "%d" , 
			   &Druidmap[RobotIndex].gun , EndOfDataPointer );

      // Now we read in the aggression rate of this droid.
      ReadValueFromString( RobotPointer , AGGRESSION_BEGIN_STRING , "%d" , 
			   &Druidmap[RobotIndex].aggression , EndOfDataPointer );

      // Now we read in the flash immunity of this droid.
      ReadValueFromString( RobotPointer , FLASHIMMUNE_BEGIN_STRING , "%d" , 
			   &Druidmap[RobotIndex].flashimmune , EndOfDataPointer );

      // Now we score to be had for destroying one droid of this type
      ReadValueFromString( RobotPointer , SCORE_BEGIN_STRING , "%d" , 
			   &Druidmap[RobotIndex].score, EndOfDataPointer );

      // Now we read in the height of this droid of this type
      ReadValueFromString( RobotPointer , HEIGHT_BEGIN_STRING , "%lf" , 
			   &Druidmap[RobotIndex].height, EndOfDataPointer );

      // Now we read in the weight of this droid type
      ReadValueFromString( RobotPointer , WEIGHT_BEGIN_STRING , "%lf" , 
			   &Druidmap[RobotIndex].weight, EndOfDataPointer );

      // Now we read in the drive of this droid of this type
      ReadValueFromString( RobotPointer , DRIVE_BEGIN_STRING , "%d" , 
			   &Druidmap[RobotIndex].drive, EndOfDataPointer );

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
      ReadValueFromString( RobotPointer , ARMAMENT_BEGIN_STRING , "%d" , 
			   &Druidmap[RobotIndex].armament , EndOfDataPointer );

      // Now we read in the AdvancedFighing flag of this droid type
      ReadValueFromString( RobotPointer , ADVANCED_FIGHTING_BEGIN_STRING , "%d" , 
			   &Druidmap[RobotIndex].AdvancedBehaviour , EndOfDataPointer );

      // Now we read in if the droid tends to go to call for reinforcements
      ReadValueFromString( RobotPointer , GO_REQUEST_REINFORCEMENTS_BEGIN_STRING , "%d" , 
			   &Druidmap[RobotIndex].CallForHelpAfterSufferingHit , EndOfDataPointer );

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
      Druidmap[i].maxspeed *= maxspeed_calibrator;
      Druidmap[i].accel *= acceleration_calibrator;
      Druidmap[i].maxenergy *= maxenergy_calibrator;
      Druidmap[i].lose_health *= energyloss_calibrator;
      Druidmap[i].aggression *= aggression_calibrator;
      Druidmap[i].score *= score_calibrator;
    }


} // int Get_Robot_Data ( void )

/*@Function============================================================
@Desc: This function loads all the constant variables of the game from
       a dat file, that should be optimally human readable.

@Ret: 
* $Function----------------------------------------------------------*/
void
Init_Game_Data ( char * Datafilename )
{
  char *fpath;
  char *Data;

#define END_OF_GAME_DAT_STRING "*** End of game.dat File ***"

  DebugPrintf (2, "\nint Init_Game_Data ( char* Datafilename ) called.");

  /* Read the whole game data to memory */
  fpath = find_file (Datafilename, MAP_DIR, FALSE);

  Data = ReadAndMallocAndTerminateFile( fpath , END_OF_GAME_DAT_STRING ) ;

  Get_General_Game_Constants( Data );

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

  //   sound_on=TRUE;

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


/*-----------------------------------------------------------------
 * @Desc: Startwerte fuer neues Spiel einstellen 
 * 
 * @Ret: 
 *
 *-----------------------------------------------------------------*/
void
InitNewMission ( char *MissionName )
{
  char *fpath;
  int i;
  char *MainMissionPointer;
  char *BriefingSectionPointer;
  char *EventSectionPointer;
  char *StartPointPointer;
  char *MissionTargetPointer;
  char *EndOfMissionTargetPointer;
  char* Liftname;
  char* Crewname;
  char* GameDataName;
  char* Shipname;
  int NumberOfStartPoints=0;
  int RealStartPoint=0;
  int StartingLevel=0;
  int StartingXPos=0;
  int StartingYPos=0;
  BFont_Info *oldfont;

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
#define END_OF_MISSION_TARGET_STRING "*** End of Mission Target ***"
#define NEXT_MISSION_NAME_STRING "After completing this mission, load mission : "

  //--------------------
  // We store the mission name in case the influ
  // gets destroyed so we know where to continue in
  // case the player doesn't want to return to the very beginning
  // but just to replay this mission.
  //
  strcpy( Previous_Mission_Name , MissionName ); 
  
  DebugPrintf (2, "\nvoid InitNewMission( char *MissionName ): real function call confirmed...");
  DebugPrintf (2, "\nA new mission is being initialized from file %s.\n" , MissionName );

  //--------------------
  //At first we do the things that must be done for all
  //missions, regardless of mission file given
  Activate_Conservative_Frame_Computation();
  Total_Frames_Passed_In_Mission=0;
  LastGotIntoBlastSound = 2;
  LastRefreshSound = 2;
  PlusExtentionsOn = FALSE;
  ThisMessageTime = 0;
  LevelDoorsNotMovedTime = 0.0;
  GameConfig.Draw_Framerate=TRUE;
  // GameConfig.Draw_Energy=FALSE;
  GameConfig.Draw_Energy=TRUE;
  GameConfig.Draw_Position=TRUE;

  RealScore = 0; // This should be done at the end of the highscore list procedure
  ShowScore = 0; // This should be done at the end of the highscore list procedure

  /* Delete all bullets and blasts */
  for (i = 0; i < MAXBULLETS; i++)
    {
      DeleteBullet ( i );
      // AllBullets[i].type = OUT;
      // AllBullets[i].mine = FALSE;
    }
  DebugPrintf (2, "\nvoid InitNewMission( ... ): All bullets have been deleted...");
  for (i = 0; i < MAXBLASTS; i++)
    {
      AllBlasts[i].phase = OUT;
      AllBlasts[i].type = OUT;
    }
  DebugPrintf (2, "\nvoid InitNewMission( ... ): All blasts have been deleted...");

  //--------------------
  //Now its time to start decoding the mission file.
  //For that, we must get it into memory first.
  //The procedure is the same as with LoadShip


  oldfont = GetCurrentFont ();

  SetCurrentFont (FPS_Display_BFont);
  printf_SDL (ne_screen, User_Rect.x + 50, -1, "Loading mission data ");

  /* Read the whole mission data to memory */
  fpath = find_file (MissionName, MAP_DIR, FALSE);

  MainMissionPointer = ReadAndMallocAndTerminateFile( fpath , END_OF_MISSION_DATA_STRING ) ;
  EndOfMissionTargetPointer = LocateStringInData ( MainMissionPointer , END_OF_MISSION_TARGET_STRING ) ;

  //--------------------
  // Now the mission file is read into memory.  That means we can start to decode the details given
  // in the body of the mission file.  

  // Now we search for the beginning of the WHOLE event section within the mission file
  EventSectionPointer = LocateStringInData ( MainMissionPointer , EVENT_SECTION_BEGIN_STRING );
  // Read in the events and triggers that can be used to cause and define something to happen
  Get_Mission_Events ( EventSectionPointer );
  printf_SDL (ne_screen, -1, -1, ".");
  DebugPrintf (2, "\nvoid InitNewMission(void): Events and triggerable actions have been successfully read in...:");

  //--------------------
  // First we extract the game physics file name from the
  // mission file and load the game data.
  //
  GameDataName = 
    ReadAndMallocStringFromData ( MainMissionPointer , GAMEDATANAME_INDICATION_STRING , "\n" ) ;

  Init_Game_Data ( GameDataName );
  printf_SDL (ne_screen, -1, -1, ".");
  //--------------------
  // Now its time to get the shipname from the mission file and
  // read the ship file into the right memory structures
  //
  Shipname = 
    ReadAndMallocStringFromData ( MainMissionPointer , SHIPNAME_INDICATION_STRING , "\n" ) ;

  if ( LoadShip ( Shipname ) == ERR )
    {
      DebugPrintf (1, "Error in LoadShip\n");
      Terminate (ERR);
    }
  printf_SDL (ne_screen, -1, -1, ".");
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
  printf_SDL (ne_screen, -1, -1, ".");
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
  Me.TextToBeDisplayed =
    ReadAndMallocStringFromData ( MainMissionPointer , "Influs mission start comment=\"" , "\"" ) ;
  Me.TextVisibleTime = 0;


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
  printf_SDL (ne_screen, -1, -1, ".");

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


  // Now that we know how many different starting points there are, we can randomly select
  // one of them and read then in this one starting point into the right structures...
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
  StartPointPointer = strstr( StartPointPointer , "XPos=" ) + strlen( "XPos=" );
  sscanf( StartPointPointer , "%d" , &StartingXPos );
  Me.pos.x=StartingXPos;
  StartPointPointer = strstr( StartPointPointer , "YPos=" ) + strlen( "YPos=" );
  sscanf( StartPointPointer , "%d" , &StartingYPos );
  Me.pos.y=StartingYPos;
  DebugPrintf ( 1 , "\nFinal starting position: Level=%d XPos=%d YPos=%d." , StartingLevel, StartingXPos, StartingYPos );
  
  //--------------------
  // At this point the position history can be initialized
  //
  InitInfluPositionHistory();
  printf_SDL (ne_screen, -1, -1, ".");
  //--------------------
  // Now we read in the mission targets for this mission
  // Several different targets may be specified simultaneously
  //

  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // jp seems to have forgotten to initialise MissionTargetPointer here...
  // we propose this temporary fix to make it run:
  MissionTargetPointer = StartPointPointer;
  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  ReadValueFromString( MissionTargetPointer , MISSION_TARGET_KILL_ALL_STRING , "%d" , 
		       &Me.mission.KillAll , EndOfMissionTargetPointer );

  ReadValueFromString( MissionTargetPointer , MISSION_TARGET_KILL_CLASS_STRING , "%d" , 
		       &Me.mission.KillClass , EndOfMissionTargetPointer );

  ReadValueFromString( MissionTargetPointer , MISSION_TARGET_KILL_ONE_STRING , "%d" , 
		       &Me.mission.KillOne , EndOfMissionTargetPointer );

  ReadValueFromString( MissionTargetPointer , MISSION_TARGET_MUST_BE_CLASS_STRING , "%d" , 
		       &Me.mission.MustBeClass , EndOfMissionTargetPointer );

  ReadValueFromString( MissionTargetPointer , MISSION_TARGET_MUST_BE_TYPE_STRING , "%d" , 
		       &Me.mission.MustBeType , EndOfMissionTargetPointer );

  ReadValueFromString( MissionTargetPointer , MISSION_TARGET_MUST_BE_ONE_STRING , "%d" , 
		       &Me.mission.MustBeOne , EndOfMissionTargetPointer );

  ReadValueFromString( MissionTargetPointer , MISSION_TARGET_MUST_REACH_POINT_X_STRING , "%d" , 
		       &Me.mission.MustReachPoint.x , EndOfMissionTargetPointer );

  ReadValueFromString( MissionTargetPointer , MISSION_TARGET_MUST_REACH_POINT_Y_STRING , "%d" , 
		       &Me.mission.MustReachPoint.y , EndOfMissionTargetPointer );

  ReadValueFromString( MissionTargetPointer , MISSION_TARGET_MUST_REACH_LEVEL_STRING , "%d" , 
		       &Me.mission.MustReachLevel , EndOfMissionTargetPointer );

  ReadValueFromString( MissionTargetPointer , MISSION_TARGET_MUST_LIVE_TIME_STRING , "%lf" , 
		       &Me.mission.MustLiveTime , EndOfMissionTargetPointer );
  printf_SDL (ne_screen, -1, -1, ".");
  //--------------------
  // After the mission targets have been successfully loaded now,
  // we need to add a pointer to the next mission, so that we will later
  // now which mission to load after this mission has been completed.
  //
  NextMissionName =
    ReadAndMallocStringFromData ( MainMissionPointer , NEXT_MISSION_NAME_STRING , "\n" ) ;

  /* Reactivate the light on alle Levels, that might have been dark */
  for (i = 0; i < curShip.num_levels; i++)
    curShip.AllLevels[i]->empty = FALSE;
  DebugPrintf (2, "\nvoid InitNewMission( ... ): All levels have been set to 'active'...");

  if (ship_off_pic) SDL_FreeSurface (ship_off_pic);
  if (ship_on_pic) SDL_FreeSurface (ship_on_pic);
  ship_off_pic= IMG_Load (find_file (ship_off_filename, GRAPHICS_DIR, TRUE));
  ship_on_pic = IMG_Load (find_file (ship_on_filename, GRAPHICS_DIR, TRUE));

  printf_SDL (ne_screen, -1, -1, " ok\n");
  SetCurrentFont (oldfont);
  //--------------------
  // We start with doing the briefing things...
  // Now we search for the beginning of the mission briefing big section NOT subsection.
  // We display the title and explanation of controls and such... 
  BriefingSectionPointer = LocateStringInData ( MainMissionPointer , MISSION_BRIEFING_BEGIN_STRING );
  Title ( BriefingSectionPointer );

  /* Den Banner fuer das Spiel anzeigen */
  ClearGraphMem();
  DisplayBanner (NULL, NULL,  BANNER_FORCE_UPDATE );

  SetTextColor (FONT_WHITE, FONT_RED);
  InitBars = TRUE;

  // Switch_Background_Music_To (COMBAT_BACKGROUND_MUSIC_SOUND);
  Switch_Background_Music_To ( CurLevel->Background_Song_Name );

  // Now that the briefing and all that is done,
  // the influence structure can be initialized for
  // the new mission:
  Me.type = DRUID001;
  Me.speed.x = 0;
  Me.speed.y = 0;
  Me.energy = Druidmap[DRUID001].maxenergy;
  Me.health = Me.energy;	/* start with max. health */
  Me.autofire = FALSE;
  Me.status = MOBILE;
  Me.phase = 0;
  Me.MissionTimeElapsed=0;
  Me.Current_Victim_Resistance_Factor=1;  // this will never get changed in "classic" FD
  
  Me.FramesOnThisLevel=0;

  /* Set colors of current level NOTE: THIS REQUIRES CurLevel TO BE INITIALIZED */
  // SetLevelColor (CurLevel->color); 

  /* set correct Influ color */
  // SetPalCol (INFLUENCEFARBWERT, Mobilecolor.rot, Mobilecolor.gruen,
  // Mobilecolor.blau);

  ShuffleEnemys(); // NOTE: THIS REQUIRES CurLevel TO BE INITIALIZED

  DebugPrintf (1, "done."); // this matches the printf at the beginning of this function
 
  return;

} /* InitNewGame */

/*-----------------------------------------------------------------
 * @Desc: This function initializes the whole Freedroid game.
 * 
 * THIS MUST NOT BE CONFUSED WITH INITNEWGAME, WHICH
 * ONLY INITIALIZES A NEW MISSION FOR THE GAME.
 *  
 * 
 *  
 *-----------------------------------------------------------------*/
void
InitFreedroid (void)
{
  int i;

  Bulletmap=NULL;  // That will cause the memory to be allocated later

  //--------------------
  // It might happen, that the uninitialized AllBullets array contains a 1
  // somewhere and that the bullet is deleted and the surface freed, where
  // it never has been allocated, resulting in a SEGFAULT.  This has never
  // happend, but for security, we add this loop to clean out these important 
  // flags.       It should be sufficient to do this here, since the flag
  // will never be set again if not Surfaces are allocated too and then they
  // can of course also be freed as well.
  //
  for ( i = 0 ; i < MAXBULLETS ; i++ )
    {
      AllBullets[i].Surfaces_were_generated = FALSE;
    }

  Overall_Average=0.041;
  SkipAFewFrames = 0;
  Me.TextVisibleTime = 0;
  // Me.TextToBeDisplayed = "Hello, I'm 001.";
  Me.TextToBeDisplayed = "Linux Kernel booted.  001 transfer-tech modules loaded.  System up and running.";

  // At first we set audio volume to maximum value.
  // This might be replaced later with values from a 
  // private user Freedroid config file.  But for now
  // this code is good enough...
  GameConfig.Current_BG_Music_Volume=1.0;
  GameConfig.Current_Sound_FX_Volume=0.5;

  GameConfig.WantedTextVisibleTime = 3;
  GameConfig.Draw_Framerate=FALSE;
  GameConfig.All_Texts_Switch=FALSE;
  GameConfig.Enemy_Hit_Text=FALSE;
  GameConfig.Enemy_Bump_Text=TRUE;
  GameConfig.Enemy_Aim_Text=TRUE;
  GameConfig.Influencer_Refresh_Text=FALSE;
  GameConfig.Influencer_Blast_Text=TRUE;
  // this is the "classic" version, so defaults are set on "classic"
  GameConfig.Theme_SubPath="classic_theme/";
  GameConfig.FullUserRect = FALSE;

  if (GameConfig.FullUserRect)
    Copy_Rect(Full_User_Rect, User_Rect);
  else
    Copy_Rect(Classic_User_Rect, User_Rect);

  Init_Video ();

  DisplayImage (find_file (NE_TITLE_PIC_FILE, GRAPHICS_DIR, FALSE)); // show title pic
  SDL_Flip(ne_screen);

  Init_Audio ();
  
  Init_Joy ();

  Init_Game_Data("freedroid.ruleset");  // load the default ruleset. This can be */
			       // overwritten from the mission file.

  // The default should be, that no rescaling of the
  // combat window at all is done.
  CurrentCombatScaleFactor = 1;

  /* 
   * Initialise random-number generator in order to make 
   * level-start etc really different at each program start
   */
  srand((unsigned int)SDL_GetTicks() ); 

  /* initialize the highscore list */
  Init_Highscores ();
 

  HideInvisibleMap = FALSE;	/* Hide invisible map-parts. Para-extension!! */

  CurLevel = NULL; // please leave this here BEFORE InitPictures
  
  /* Now fill the pictures correctly to the structs */
  if (!InitPictures ())
    {		
      DebugPrintf (1, "\n Error in InitPictures reported back...\n");
      Terminate(ERR);
    }

  // Initialisieren der Schildbilder
  //  GetShieldBlocks ();

  return;
} /* InitFreedroid() */

/*-----------------------------------------------------------------
 * @Desc: This function does the mission briefing.  It assumes, 
 *  that a mission file has already been successfully loaded into
 *  memory.  The briefing texts will be extracted and displayed in
 *  scrolling font.
 * 
 *-----------------------------------------------------------------*/
void
Title ( char *MissionBriefingPointer )
{
  char* NextSubsectionStartPointer;
  char* PreparedBriefingText;
  char* TerminationPointer;
  char* TitlePictureName;
  char* TitleSongName;
  int ThisTextLength;
  SDL_Rect rect;
#define BRIEFING_TITLE_PICTURE_STRING "The title picture in the graphics subdirectory for this mission is : "
#define BRIEFING_TITLE_SONG_STRING "The title song in the sound subdirectory for this mission is : "
#define NEXT_BRIEFING_SUBSECTION_START_STRING "* New Mission Briefing Text Subsection *"
#define END_OF_BRIEFING_SUBSECTION_STRING "* End of Mission Briefing Text Subsection *"

  // STRANGE!! This command will be silently ignored by SDL?
  // WHY?? DONT KNOW!!!
  // Play_Sound ( CLASSICAL_BEEP_BEEP_BACKGROUND_MUSIC );
  // Play_Sound ( CLASSICAL_BEEP_BEEP_BACKGROUND_MUSIC );
  // Switch_Background_Music_To ( COMBAT_BACKGROUND_MUSIC_SOUND );

  TitleSongName = ReadAndMallocStringFromData ( MissionBriefingPointer, BRIEFING_TITLE_SONG_STRING , "\n" ) ;

  Switch_Background_Music_To ( TitleSongName );

  TitlePictureName = ReadAndMallocStringFromData ( MissionBriefingPointer, BRIEFING_TITLE_PICTURE_STRING , "\n" ) ;

  SDL_SetClipRect ( ne_screen, NULL );
  // DisplayImage ( find_file(TitlePictureName, GRAPHICS_DIR, FALSE) );
  // SDL_Flip (ne_screen);

  DisplayImage ( find_file(TitlePictureName,GRAPHICS_DIR, FALSE) );
  MakeGridOnScreen( (SDL_Rect*) &Full_Screen_Rect );
  Me.status=BRIEFING;
  DisplayBanner (NULL, NULL,  BANNER_FORCE_UPDATE ); 
  SDL_Flip (ne_screen);

  // ClearGraphMem ();
  // DisplayBanner (NULL, NULL,  BANNER_FORCE_UPDATE ); 

  // SetTextColor (FONT_BLACK, FONT_RED);

  // SetCurrentFont( FPS_Display_BFont );
  SetCurrentFont( Para_BFont );


  // Next we display all the subsections of the briefing section
  // with scrolling font
  NextSubsectionStartPointer = MissionBriefingPointer;
  while (1)
    {
      NextSubsectionStartPointer = strstr (NextSubsectionStartPointer, 
					   NEXT_BRIEFING_SUBSECTION_START_STRING);
      if (NextSubsectionStartPointer == NULL)
	break;

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

      Copy_Rect(Full_User_Rect, rect);
      rect.x += 10;
      rect.w -= 10; //leave some border
      if (ScrollText ( PreparedBriefingText, &rect) == 1)
	break;  // User pressed 'fire'
 
      free ( PreparedBriefingText );
    }

  return;

} /* Title() */

/*@Function============================================================
@Desc: Diese Prozedur ist fuer die Introduction in das Spiel verantwortlich. Im
   Moment beschr„nkt sich ihre Funktion auf das Laden und anzeigen eines
   Titelbildes, das dann ausgeblendet wird.

@Ret: keiner
@Int: keiner
* $Function----------------------------------------------------------*/
void
EndTitle (void)
{
  SDL_Rect rect;
  Uint32 now;

  Switch_Background_Music_To ( DebriefingSong );

  ShowScore = (long)RealScore;
  Me.status = VICTORY;
  DisplayBanner (NULL, NULL,  BANNER_FORCE_UPDATE );

  SetCurrentFont( Para_BFont);

  while (SpacePressed());

  now=SDL_GetTicks();

  while ( (SDL_GetTicks() - now < WAIT_AFTER_KILLED) )
    {
      DisplayBanner (NULL, NULL,  0 );
      ExplodeBlasts ();
      MoveBullets ();
      Assemble_Combat_Picture ( DO_SCREEN_UPDATE );
    }

  Copy_Rect(Full_User_Rect, rect);
  MakeGridOnScreen (&rect);
  rect.x += 10;
  rect.w -= 20;  //leave some border
  ScrollText (DebriefingText , &rect);

  while ( SpacePressed() );

} /* EndTitle() */

/*@Function============================================================
@Desc: Show end-screen 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
ThouArtDefeated (void)
{
  int now;

  Me.status = TERMINATED;
  ThouArtDefeatedSound ();
  ExplodeInfluencer ();

  while (SpacePressed());

  now=SDL_GetTicks();

  while ( (SDL_GetTicks() - now < WAIT_AFTER_KILLED) && (!SpacePressed()))
    {
      DisplayBanner (NULL, NULL,  0 );
      ExplodeBlasts ();
      MoveBullets ();
      Assemble_Combat_Picture ( DO_SCREEN_UPDATE );
    }
  
  if (!SpacePressed()) 
    white_noise (ne_screen, &User_Rect, WAIT_AFTER_KILLED);

  Assemble_Combat_Picture ( DO_SCREEN_UPDATE );
  MakeGridOnScreen (&User_Rect);

  ShowRobotPicture (UserCenter_x -70, UserCenter_y - 80, DRUID999);

  SetCurrentFont (Para_BFont);
  DisplayText ("Transmission", UserCenter_x -90, UserCenter_y - 100, &User_Rect);
  DisplayText ("Terminated", UserCenter_x -90, UserCenter_y + 100, &User_Rect);
  printf_SDL(ne_screen, -1, -1, "\n");
  SDL_Flip (ne_screen);
  now=SDL_GetTicks();
  while (  (SDL_GetTicks() - now < SHOW_WAIT) && (!SpacePressed()) );

  update_highscores ();

  GameOver = TRUE;

  return;
} // void ThouArtDefeated(void)

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
ThouArtVictorious (void)
{
  ShipEmptyCounter = WAIT_SHIPEMPTY;
  GameOver = TRUE;		/*  */

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

  //--------------------
  // We set up a cheat code, so that one can easily 'complete' a mission
  //
  if ( MPressed() && Alt_Was_Pressed()
       && Ctrl_Was_Pressed() && Shift_Was_Pressed() )
    goto victory;


#define MIS_COMPLETE_DEBUG 3

  if ( Me.mission.KillOne != (-1) )
    {
      for ( Robot_Counter=0 ; Robot_Counter < MAX_ENEMYS_ON_SHIP ; Robot_Counter++ )
	{
	  if ( ( AllEnemys[Robot_Counter].energy > 0 ) && 
	       ( AllEnemys[Robot_Counter].Status != OUT ) && 
	       ( AllEnemys[Robot_Counter].Marker == Me.mission.KillOne ) )
	    {
	      DebugPrintf ( MIS_COMPLETE_DEBUG , "\nOne of the marked droids is still alive...");
	      // fflush(stdout);
	      return;
	    }
	}
    }

  if ( Me.mission.KillAll != (-1) )
    {
      for ( Robot_Counter=0 ; Robot_Counter < MAX_ENEMYS_ON_SHIP ; Robot_Counter++ )
	{
	  if ( ( AllEnemys[Robot_Counter].energy > 0 ) && ( AllEnemys[Robot_Counter].Friendly == FALSE ) )
	    {
	      DebugPrintf ( MIS_COMPLETE_DEBUG , "\nThere are some robots still alive, and you should kill them all...");
	      return;
	    }
	}
    }

  if ( Me.mission.KillClass != (-1) )
    {
      for ( Robot_Counter=0 ; Robot_Counter < MAX_ENEMYS_ON_SHIP ; Robot_Counter++ )
	{
	  if ( ( AllEnemys[Robot_Counter].energy > 0 ) && 
	       ( AllEnemys[Robot_Counter].Status != OUT ) && 
	       ( Druidmap[AllEnemys[Robot_Counter].type].class == Me.mission.KillClass ) ) 
	    {
	      DebugPrintf ( MIS_COMPLETE_DEBUG , "\nOne of that class is still alive: Nr=%d Lev=%d X=%f Y=%f." , 
			   Robot_Counter , AllEnemys[Robot_Counter].levelnum , 
			   AllEnemys[Robot_Counter].pos.x , AllEnemys[Robot_Counter].pos.y );
	      return;
	    }
	}
    }

  if ( Me.mission.MustBeClass != (-1) )
    {
      DebugPrintf ( MIS_COMPLETE_DEBUG , "\nMe.type is now: %d.", Me.type );
      if ( Druidmap[Me.type].class != Me.mission.MustBeClass ) 
	{
	  DebugPrintf ( MIS_COMPLETE_DEBUG , "\nMe.class does not match...");
	  return;
	}
    }

  if ( Me.mission.MustBeType != (-1) )
    {
      DebugPrintf ( MIS_COMPLETE_DEBUG , "\nMe.type is now: %d.", Me.type );
      if ( Me.type != Me.mission.MustBeType ) 
	{
	  DebugPrintf ( MIS_COMPLETE_DEBUG , "\nMe.type does not match...");
	  return;
	}
    }

  if ( Me.mission.MustReachLevel != (-1) )
    {
      if ( CurLevel->levelnum != Me.mission.MustReachLevel ) 
	{
	  DebugPrintf ( MIS_COMPLETE_DEBUG , "\nLevel number does not match...");
	  return;
	}
    }

  if ( Me.mission.MustReachPoint.x != (-1) )
    {
      if ( Me.pos.x != Me.mission.MustReachPoint.x ) 
	{
	  DebugPrintf ( MIS_COMPLETE_DEBUG , "\nX coordinate does not match...");
	  return;
	}
    }

  if ( Me.mission.MustReachPoint.y != (-1) )
    {
      if ( Me.pos.y != Me.mission.MustReachPoint.y ) 
	{
	  DebugPrintf ( MIS_COMPLETE_DEBUG , "\nY coordinate does not match..."); 
	  return;
	}
    }

  if ( Me.mission.MustLiveTime != (-1) )
    {
      if ( Me.MissionTimeElapsed < Me.mission.MustLiveTime ) 
	{
	  DebugPrintf ( MIS_COMPLETE_DEBUG , "\nTime Limit not yet reached...");
	  return;
	}
    }

  if ( Me.mission.MustBeOne != (-1) )
    {
      if ( Me.Marker != Me.mission.MustBeOne ) 
	{
	  DebugPrintf ( MIS_COMPLETE_DEBUG , "\nYou're not yet one of the marked ones...");
	  return;
	}
    }


 victory:
  RealScore += MISSION_COMPLETE_BONUS;
  EndTitle();
  update_highscores();

  InitNewMission ( NextMissionName);
  
} // void CheckIfMissionIsComplete



#undef _init_c



