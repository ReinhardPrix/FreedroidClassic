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


/* Scroll- Fenster */
#define SCROLLSTARTX		USERFENSTERPOSX
#define SCROLLSTARTY		SCREENHOEHE

void Init_Game_Data( char* Datafilename );
void Get_Bullet_Data ( char* DataPointer );
char* DebriefingText;
char* NextMissionName;

/*@Function============================================================
@Desc: This function loads all the constant variables of the game from
       a dat file, that should be optimally human readable.

@Ret: 
* $Function----------------------------------------------------------*/
void
Get_General_Game_Constants ( void* DataPointer )
{
  char *ConstantPointer;
  char *ValuePointer;  // we use ValuePointer while RobotPointer stays still to allow for
                       // interchanging of the order of appearance of parameters in the game.dat file

#define CONSTANTS_SECTION_BEGIN_STRING "*** Start of General Game Constants Section: ***"
#define CONSTANTS_SECTION_END_STRING "*** End of General Game Constants Section: ***"
#define COLLISION_LOSE_ENERGY_CALIBRATOR_STRING "Energy-Loss-factor for Collisions of Influ with hostile robots="
#define BLAST_RADIUS_SPECIFICATION_STRING "Radius of explosions (as far as damage is concerned) in multiples of tiles="
#define DRUID_RADIUS_X_SPECIFICATION_STRING "'Radius' of droids in x direction="
#define DRUID_RADIUS_Y_SPECIFICATION_STRING "'Radius' of droids in x direction="
#define BLAST_DAMAGE_SPECIFICATION_STRING "Amount of damage done by contact to a blast per second of time="
#define TIME_FOR_DOOR_MOVEMENT_SPECIFICATION_STRING "Time for the doors to move by one subphase of their movement="


  if ( (ConstantPointer = strstr ( DataPointer , CONSTANTS_SECTION_BEGIN_STRING ) ) == NULL)
    {
      DebugPrintf( 0 , "\n\nBegin of General Game Constants Section string not found...\n\nTerminating...\n\n");
      Terminate(ERR);
    }
  else
    {
      DebugPrintf ( 2 , "\n\nBegin of General Game Constants Section found. Good.");  
      // fflush(stdout);
    }
  
  if ( ( strstr ( DataPointer , CONSTANTS_SECTION_END_STRING ) ) == NULL)
    {
      DebugPrintf( 0 , "\n\nEnd of General Game Constants Section string not found...\n\nTerminating...\n\n");
      Terminate(ERR);
    }
  else
    {
      DebugPrintf ( 2 , "\n\nEnd of General Game Constants Section found. Good.");  
      fflush(stdout);
    }
  
  DebugPrintf ( 2 , "\n\nStarting to read contents of General Game Constants section\n\n");

  // Now we read in the speed calibration factor for all bullets
  if ( (ValuePointer = strstr ( DataPointer, COLLISION_LOSE_ENERGY_CALIBRATOR_STRING )) == NULL )
    {
      DebugPrintf( 0 , "\nERROR! NO COLLISION LOSE ENERGY CALIBRATOR ENTRY FOUND! TERMINATING!");
      Terminate(ERR);
    }
  else
    {
      ValuePointer += strlen ( COLLISION_LOSE_ENERGY_CALIBRATOR_STRING );
      sscanf ( ValuePointer , "%lf" , &collision_lose_energy_calibrator );
      DebugPrintf( 1 , "\ncollision_lose_energy_calibrator reads:  %f" , collision_lose_energy_calibrator );
    }

  // Now we read in the blast radius
  if ( (ValuePointer = strstr ( DataPointer, BLAST_RADIUS_SPECIFICATION_STRING )) == NULL )
    {
      DebugPrintf( 0 , "\nERROR! NO BLAST RADIUS SPECIFICATION ENTRY FOUND! TERMINATING!");
      Terminate(ERR);
    }
  else
    {
      ValuePointer += strlen ( BLAST_RADIUS_SPECIFICATION_STRING );
      sscanf ( ValuePointer , "%lf" , &Blast_Radius );
      DebugPrintf( 1 , "\nBlast_Radius now reads:  %lf" , Blast_Radius );
    }

  // Now we read in the druid 'radius' in x direction
  if ( (ValuePointer = strstr ( DataPointer, DRUID_RADIUS_X_SPECIFICATION_STRING )) == NULL )
    {
      DebugPrintf( 0 , "\nERROR! NO DRUID RADIUS X SPECIFICATION ENTRY FOUND! TERMINATING!");
      Terminate(ERR);
    }
  else
    {
      ValuePointer += strlen ( DRUID_RADIUS_X_SPECIFICATION_STRING );
      sscanf ( ValuePointer , "%lf" , &Druid_Radius_X );
      DebugPrintf( 1 , "\nDruid_Radius_X now reads:  %f" , Druid_Radius_X );
    }
  if ( (ValuePointer = strstr ( DataPointer, DRUID_RADIUS_Y_SPECIFICATION_STRING )) == NULL )
    {
      DebugPrintf( 0 , "\nERROR! NO DRUID RADIUS Y SPECIFICATION ENTRY FOUND! TERMINATING!");
      Terminate(ERR);
    }
  else
    {
      ValuePointer += strlen ( DRUID_RADIUS_Y_SPECIFICATION_STRING );
      sscanf ( ValuePointer , "%lf" , &Druid_Radius_Y );
      DebugPrintf( 1 , "\nDruid_Radius_X now reads:  %f" , Druid_Radius_Y );
    }

  // Now we read in the blast damage amount per 'second' of contact with the blast
  if ( (ValuePointer = strstr ( DataPointer, BLAST_DAMAGE_SPECIFICATION_STRING )) == NULL )
    {
      DebugPrintf( 0 , "\nERROR! NO BLAST DAMAGE SPECIFICATION ENTRY FOUND! TERMINATING!");
      Terminate(ERR);
    }
  else
    {
      ValuePointer += strlen ( BLAST_DAMAGE_SPECIFICATION_STRING );
      sscanf ( ValuePointer , "%lf" , &Blast_Damage_Per_Second );
      DebugPrintf( 1 , "\nBlast_Damage_Per_Second now reads:  %f" , Blast_Damage_Per_Second );
    }

  // Now we read in the time is takes for the door to move one phase 
  if ( ( ValuePointer = strstr ( DataPointer, TIME_FOR_DOOR_MOVEMENT_SPECIFICATION_STRING ) ) == NULL )
    {
      DebugPrintf( 0 , "\nERROR! NO DOOR MOVEMENT SPEED SPECIFICATION ENTRY FOUND! TERMINATING!" );
      Terminate(ERR);
    }
  else
    {
      ValuePointer += strlen ( TIME_FOR_DOOR_MOVEMENT_SPECIFICATION_STRING );
      sscanf ( ValuePointer , "%lf" , &Time_For_Each_Phase_Of_Door_Movement );
      DebugPrintf( 1 , "\nTime_For_Each_Phase_Of_Door_Movement now reads:  %f" , Time_For_Each_Phase_Of_Door_Movement );
    }


} // void Get_General_Game_Constants ( void* DataPointer )

/*----------------------------------------------------------------------
 *
 *
 *
 ----------------------------------------------------------------------*/

void 
Get_Bullet_Data ( char* DataPointer )
{
  char *BulletPointer;
  char *CountBulletsPointer;
  char *ValuePointer;  // we use ValuePointer while RobotPointer stays still to allow for
                       // interchanging of the order of appearance of parameters in the game.dat file
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
#define BULLET_NUMBER_OF_PHASES_BEGIN_STRING "Number of different phases that were designed for this bullet type :"
#define BULLET_BLAST_TYPE_CAUSED_BEGIN_STRING "Type of blast this bullet causes when crashing e.g. against a wall :"

#define BULLET_SPEED_CALIBRATOR_STRING "Common factor for all bullet's speed values: "
#define BULLET_DAMAGE_CALIBRATOR_STRING "Common factor for all bullet's damage values: "

  if ( (BulletPointer = strstr ( DataPointer , BULLET_SECTION_BEGIN_STRING ) ) == NULL)
    {
      DebugPrintf(1, "\n\nBegin of Bullet Data Section string not found...\n\nTerminating...\n\n");
      Terminate(ERR);
    }
  else
    {
      DebugPrintf (2, "\n\nBegin of Bullet Data Section found. Good.");  
      // fflush(stdout);
    }
  
  if ( ( strstr ( DataPointer , BULLET_SECTION_END_STRING ) ) == NULL)
    {
      DebugPrintf(1, "\n\nEnd of Bullet Data Section string not found...\n\nTerminating...\n\n");
      Terminate(ERR);
    }
  else
    {
      DebugPrintf (2, "\n\nEnd of Bullet Data Section found. Good.");  
      fflush(stdout);
    }

  DebugPrintf (2, "\n\nStarting to read bullet data...\n\n");
  //--------------------
  // At first, we must allocate memory for the droid specifications.
  // How much?  That depends on the number of droids defined in game.dat.
  // So we have to count those first.  ok.  lets do it.

  CountBulletsPointer=DataPointer;
  Number_Of_Bullet_Types=0;
  while ( ( CountBulletsPointer = strstr ( CountBulletsPointer, NEW_BULLET_TYPE_BEGIN_STRING)) != NULL)
    {
      CountBulletsPointer += strlen ( NEW_BULLET_TYPE_BEGIN_STRING );
      Number_Of_Bullet_Types++;
    }

  // Not that we know how many bullets are defined in game.dat, we can allocate
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
      if ( (ValuePointer = strstr ( BulletPointer, BULLET_RECHARGE_TIME_BEGIN_STRING )) == NULL )
	{
	  DebugPrintf(0, "\nERROR! NO BULLET RECHARGE TIME ENTRY FOUND! TERMINATING!");
	  Terminate(ERR);
	}
      else
	{
	  ValuePointer += strlen ( BULLET_RECHARGE_TIME_BEGIN_STRING );
	  sscanf ( ValuePointer , "%lf" , &Bulletmap[BulletIndex].recharging_time );
	  DebugPrintf( 1 , "\nrecharging time now reads: %f. " , Bulletmap[BulletIndex].recharging_time );
	}

      // Now we read in the maximal speed this type of bullet can go.
      if ( (ValuePointer = strstr ( BulletPointer, BULLET_SPEED_BEGIN_STRING )) == NULL )
	{
	  DebugPrintf(0, "\nERROR! NO BULLET SPEED ENTRY FOUND! TERMINATING!");
	  Terminate(ERR);
	}
      else
	{
	  ValuePointer += strlen ( BULLET_SPEED_BEGIN_STRING );
	  sscanf ( ValuePointer , "%lf" , &Bulletmap[BulletIndex].speed );
	  DebugPrintf( 1 , "\nbullet speed now reads: %f. " , Bulletmap[BulletIndex].speed );
	}

      // Now we read in the damage this bullet can do
      if ( (ValuePointer = strstr ( BulletPointer, BULLET_DAMAGE_BEGIN_STRING )) == NULL )
	{
	  DebugPrintf(0, "\nERROR! NO BULLET DAMAGE ENTRY FOUND! TERMINATING!");
	  Terminate(ERR);
	}
      else
	{
	  ValuePointer += strlen ( BULLET_DAMAGE_BEGIN_STRING );
	  sscanf ( ValuePointer , "%d" , &Bulletmap[BulletIndex].damage );
	  DebugPrintf( 1 , "\nbullet damage now reads: %d. " , Bulletmap[BulletIndex].damage );
	}

      // Now we read in the number of phases that are designed for this bullet type
      if ( (ValuePointer = strstr ( BulletPointer, BULLET_NUMBER_OF_PHASES_BEGIN_STRING )) == NULL )
	{
	  DebugPrintf(0, "\nERROR! NO NUMBER OF PHASES FOR THIS BULLET ENTRY FOUND! TERMINATING!");
	  Terminate(ERR);
	}
      else
	{
	  ValuePointer += strlen ( BULLET_NUMBER_OF_PHASES_BEGIN_STRING );
	  sscanf ( ValuePointer , "%d" , &Bulletmap[BulletIndex].phases );
	  DebugPrintf( 1 , "\nbullet phases now reads: %d. " , Bulletmap[BulletIndex].phases );
	}

      // Now we read in the type of blast this bullet will cause when crashing e.g. against the wall
      if ( (ValuePointer = strstr ( BulletPointer, BULLET_BLAST_TYPE_CAUSED_BEGIN_STRING )) == NULL )
	{
	  DebugPrintf(0, "\nERROR! NO TYPE OF BLAST CAUSED BY THIS BULLET ENTRY FOUND! TERMINATING!");
	  Terminate(ERR);
	}
      else
	{
	  ValuePointer += strlen ( BULLET_BLAST_TYPE_CAUSED_BEGIN_STRING );
	  sscanf ( ValuePointer , "%d" , &Bulletmap[BulletIndex].blast );
	  DebugPrintf( 1 , "\nbullet causes blast of type : %d. " , Bulletmap[BulletIndex].blast );
	}

      BulletIndex++;
    }

  //--------------------
  // Now that the detailed values for the bullets have been read in,
  // we now read in the general calibration contants and after that
  // the start to apply them right now, so they also take effect.
  
  DebugPrintf (1, "\n\nStarting to read bullet calibration section\n\n");
  // Now we read in the speed calibration factor for all bullets
  if ( (ValuePointer = strstr ( DataPointer, BULLET_SPEED_CALIBRATOR_STRING )) == NULL )
    {
      DebugPrintf( 0 , "\nERROR! NO BULLET SPEED CALIBRATOR ENTRY FOUND! TERMINATING!");
      Terminate(ERR);
    }
  else
    {
      ValuePointer += strlen ( BULLET_SPEED_CALIBRATOR_STRING );
      sscanf ( ValuePointer , "%lf" , &bullet_speed_calibrator );
    }

  // Now we read in the damage calibration factor for all bullets
  if ( (ValuePointer = strstr ( DataPointer, BULLET_DAMAGE_CALIBRATOR_STRING )) == NULL )
    {
      DebugPrintf( 0 , "\nERROR! NO BULLET DAMAGE CALIBRATOR ENTRY FOUND! TERMINATING!");
      Terminate(ERR);
    }
  else
    {
      ValuePointer += strlen ( BULLET_SPEED_CALIBRATOR_STRING );
      sscanf ( ValuePointer , "%lf" , &bullet_damage_calibrator );
    }

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
  char *SayString;
  char *EventPointer;
  char *ValuePointer;  // we use ValuePointer while EventPointer stays still to allow for
                       // interchanging of the order of appearance of parameters in the game.dat file
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
      DebugPrintf(2, "\nBegin of a new Event Action Section found. Good. ");
      EventPointer += strlen( EVENT_ACTION_BEGIN_STRING ) + 1;

      if ( ( strstr ( EventSectionPointer , EVENT_ACTION_END_STRING ) ) == NULL)
	{
	  DebugPrintf(0, "\n\nEnd of Event Action Section string not found...\n\nTerminating...\n\n");
	  Terminate(ERR);
	}
      else
	{
	  DebugPrintf (2, "\n\nEnd of this Event Action Section found. Good.");  
	}
  
      DebugPrintf (2, "\n\nStarting to read details of this event action section\n\n");

      //--------------------
      // Now we decode the details of this event action section
      //

      // FIRST OF ALL, WE NEED TO KNOW AT WHICH INDEX WE MUST MODIFY OUR STRUTURE.
      // SO FIRST WE READ IN THE EVENT ACTIONS INDEX NUMBER
      if ( (ValuePointer = strstr ( EventPointer, EVENT_ACTION_INDEX_NUMBER_TO_USE_STRING )) == NULL )
	{
	  DebugPrintf( 0 , "\nERROR! NO EVENT ACTION INDEX NUMBER TO USE ENTRY FOUND! TERMINATING!");
	  Terminate(ERR);
	}
      else
	{
	  ValuePointer += strlen ( EVENT_ACTION_INDEX_NUMBER_TO_USE_STRING );
	  sscanf ( ValuePointer , "%d" , &EventActionNumber );
	  DebugPrintf( 2 , "\nNEW EVENT ACTION NUMBER=%d" , EventActionNumber );
	}

      // Now we read in the map changing position in x coordinates
      if ( (ValuePointer = strstr ( EventPointer, EVENT_ACTION_MAPCHANGE_POS_X_STRING )) == NULL )
	{
	  DebugPrintf( 0 , "\nERROR! NO EVENT ACTION MAPCHANGE POSITION X ENTRY FOUND! TERMINATING!");
	  Terminate(ERR);
	}
      else
	{
	  ValuePointer += strlen ( EVENT_ACTION_MAPCHANGE_POS_X_STRING );
	  sscanf ( ValuePointer , "%d" , &AllTriggeredActions[ EventActionNumber ].ChangeMapLocation.x );
	  DebugPrintf( 2 , "\nMapchange at position x=%d" , AllTriggeredActions[ EventActionNumber].ChangeMapLocation.x );
	}

      // Now we read in the map changing position in y coordinates
      if ( (ValuePointer = strstr ( EventPointer, EVENT_ACTION_MAPCHANGE_POS_Y_STRING )) == NULL )
	{
	  DebugPrintf( 0 , "\nERROR! NO EVENT ACTION MAPCHANGE POSITION Y ENTRY FOUND! TERMINATING!");
	  Terminate(ERR);
	}
      else
	{
	  ValuePointer += strlen ( EVENT_ACTION_MAPCHANGE_POS_Y_STRING );
	  sscanf ( ValuePointer , "%d" , &AllTriggeredActions[ EventActionNumber ].ChangeMapLocation.y );
	  DebugPrintf( 2 , "\nMapchange at position y=%d" , AllTriggeredActions[ EventActionNumber].ChangeMapLocation.y );
	}

      // Now we read in the map changing position level
      if ( (ValuePointer = strstr ( EventPointer, EVENT_ACTION_MAPCHANGE_MAPLEVEL_STRING )) == NULL )
	{
	  DebugPrintf( 0 , "\nERROR! NO EVENT ACTION MAPCHANGE MAPLEVEL ENTRY FOUND! TERMINATING!");
	  Terminate(ERR);
	}
      else
	{
	  ValuePointer += strlen ( EVENT_ACTION_MAPCHANGE_MAPLEVEL_STRING );
	  sscanf ( ValuePointer , "%d" , &AllTriggeredActions[ EventActionNumber ].ChangeMapLevel );
	  DebugPrintf( 2 , "\nMapchange at Level=%d" , AllTriggeredActions[ EventActionNumber].ChangeMapLevel );
	}

      // Now we read in the new value for that map tile
      if ( (ValuePointer = strstr ( EventPointer, EVENT_ACTION_MAPCHANGE_TO_WHAT_STRING )) == NULL )
	{
	  DebugPrintf(1, "\nERROR! NO EVENT ACTION MAPCHANGE TO WHAT ENTRY FOUND! TERMINATING!");
	  Terminate(ERR);
	}
      else
	{
	  ValuePointer += strlen ( EVENT_ACTION_MAPCHANGE_TO_WHAT_STRING );
	  sscanf ( ValuePointer , "%d" , &AllTriggeredActions[ EventActionNumber ].ChangeMapTo );
	  DebugPrintf( 1 , "\nChange map to new value that is=%d" , AllTriggeredActions[ EventActionNumber].ChangeMapTo );
	}

      // Now we read in if the influencer is to say something
      if ( (ValuePointer = strstr ( EventPointer, EVENT_ACTION_INFLUENCER_SAY_SOMETHING )) == NULL )
	{
	  DebugPrintf( 0 , "\nERROR! NO EVENT ACTION INFLUENCER SAY SOMETHING ENTRY FOUND! TERMINATING!");
	  Terminate(ERR);
	}
      else
	{
	  ValuePointer += strlen ( EVENT_ACTION_INFLUENCER_SAY_SOMETHING );
	  sscanf ( ValuePointer , "%d" , &AllTriggeredActions[ EventActionNumber ].InfluencerSaySomething );
	  DebugPrintf( 1 , "\nInfluencer say something at tiggering of event is=%d" , AllTriggeredActions[ EventActionNumber].InfluencerSaySomething );
	}

      // Now we read in if the text for the influencer to say
      if ( (ValuePointer = strstr ( EventPointer, EVENT_ACTION_INFLUENCER_SAY_TEXT )) == NULL )
	{
	  DebugPrintf( 0 , "\nERROR! NO EVENT ACTION INFLUENCER SAY TEXT ENTRY FOUND! TERMINATING!");
	  Terminate(ERR);
	}
      else
	{
	  ValuePointer += strlen ( EVENT_ACTION_INFLUENCER_SAY_TEXT );
	  SayString = strstr( ValuePointer , "\"" );
	  if (SayString == NULL )
	    {
	      DebugPrintf( 0 , "\nERROR! INFLUENCER SAY TEXT STRING NOT TERMINATED!!!! TERMINATING!");
	      Terminate(ERR);
	    }
	  // Now we allocate memory and copy the string...
	  AllTriggeredActions[ EventActionNumber ].InfluencerSayText=MyMalloc( SayString - ValuePointer + 10 );
	  strncpy ( AllTriggeredActions[ EventActionNumber ].InfluencerSayText , ValuePointer, SayString - ValuePointer );
	  AllTriggeredActions[ EventActionNumber ].InfluencerSayText[SayString - ValuePointer ] = 0;

	  DebugPrintf( 1 , "\nInfluencer say text is:%s" , AllTriggeredActions[ EventActionNumber].InfluencerSayText );
	}

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

      if ( ( strstr ( EventSectionPointer , EVENT_TRIGGER_END_STRING ) ) == NULL)
	{
	  DebugPrintf(0, "\n\nEnd of Event Trigger Section string not found...\n\nTerminating...\n\n");
	  Terminate(ERR);
	}
      else
	{
	  DebugPrintf (2, "\n\nEnd of this Event Trigger Section found. Good.");  
	  fflush(stdout);
	}
  
      DebugPrintf (2, "\n\nStarting to read details of this event trigger section\n\n");

      //--------------------
      // Now we decode the details of this event trigger section
      //

      // Now we read in the triggering position in x coordinates
      if ( (ValuePointer = strstr ( EventPointer, EVENT_TRIGGER_POS_X_STRING )) == NULL )
	{
	  DebugPrintf(0, "\nERROR! NO EVENT TRIGGER POSITION X ENTRY FOUND! TERMINATING!");
	  Terminate(ERR);
	}
      else
	{
	  ValuePointer += strlen ( EVENT_TRIGGER_POS_X_STRING );
	  sscanf ( ValuePointer , "%d" , &AllEventTriggers[ EventTriggerNumber ].Influ_Must_Be_At_Point.x );
	  DebugPrintf( 1 , "\nEvent Trigger Position x is=%d" , AllEventTriggers[ EventTriggerNumber ].Influ_Must_Be_At_Point.x );
	}

      // Now we read in the triggering position in x coordinates
      if ( (ValuePointer = strstr ( EventPointer, EVENT_TRIGGER_POS_Y_STRING )) == NULL )
	{
	  DebugPrintf(0, "\nERROR! NO EVENT TRIGGER POSITION Y ENTRY FOUND! TERMINATING!");
	  Terminate(ERR);
	}
      else
	{
	  ValuePointer += strlen ( EVENT_TRIGGER_POS_Y_STRING );
	  sscanf ( ValuePointer , "%d" , &AllEventTriggers[ EventTriggerNumber ].Influ_Must_Be_At_Point.y );
	  DebugPrintf( 1 , "\nEvent Trigger Position x is=%d" , AllEventTriggers[ EventTriggerNumber ].Influ_Must_Be_At_Point.y );
	}

      // Now we read in the triggering position in levels
      if ( (ValuePointer = strstr ( EventPointer, EVENT_TRIGGER_POS_MAPLEVEL_STRING )) == NULL )
	{
	  DebugPrintf( 0, "\nERROR! NO EVENT TRIGGER POSITION MAPLEVEL ENTRY FOUND! TERMINATING!");
	  Terminate(ERR);
	}
      else
	{
	  ValuePointer += strlen ( EVENT_TRIGGER_POS_MAPLEVEL_STRING );
	  sscanf ( ValuePointer , "%d" , &AllEventTriggers[ EventTriggerNumber ].Influ_Must_Be_At_Level );
	  DebugPrintf( 1 , "\nEvent Trigger Position Level is=%d" , AllEventTriggers[ EventTriggerNumber ].Influ_Must_Be_At_Level );
	}

      // Now we read whether or not to delete the trigger after being triggers
      if ( (ValuePointer = strstr ( EventPointer, EVENT_TRIGGER_DELETED_AFTER_TRIGGERING )) == NULL )
	{
	  DebugPrintf(0, "\nERROR! NO EVENT TRIGGER DELETE AFTER TRIGGERING ENTRY FOUND! TERMINATING!");
	  Terminate(ERR);
	}
      else
	{
	  ValuePointer += strlen ( EVENT_TRIGGER_DELETED_AFTER_TRIGGERING );
	  sscanf ( ValuePointer , "%d" , &AllEventTriggers[ EventTriggerNumber ].DeleteTriggerAfterExecution );
	  DebugPrintf( 1 , "\nEvent Trigger Deleted after execution is=%d" , AllEventTriggers[ EventTriggerNumber ].DeleteTriggerAfterExecution );
	}

      // Now we read in the action to be invoked by this trigger
      if ( (ValuePointer = strstr ( EventPointer, EVENT_TRIGGER_WHICH_ACTION_STRING )) == NULL )
	{
	  DebugPrintf(0, "\nERROR! NO EVENT TO TRIGGER ENTRY FOUND! TERMINATING!");
	  Terminate(ERR);
	}
      else
	{
	  ValuePointer += strlen ( EVENT_TRIGGER_WHICH_ACTION_STRING );
	  sscanf ( ValuePointer , "%d" , &AllEventTriggers[ EventTriggerNumber ].EventNumber );
	  DebugPrintf( 1 , "\nEvent Trigger causes Action number is=%d" , AllEventTriggers[ EventTriggerNumber ].EventNumber );
	}

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
  char *ValuePointer;  // we use ValuePointer while RobotPointer stays still to allow for
                       // interchanging of the order of appearance of parameters in the game.dat file
  char *CountRobotsPointer;
  int StringLength;
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
#define NOTES_BEGIN_STRING "Notes concerning this droid : "

  if ( (RobotPointer = strstr ( DataPointer , ROBOT_SECTION_BEGIN_STRING ) ) == NULL)
    {
      DebugPrintf(1, "\n\nBegin of Robot Data Section not found...\n\nTerminating...\n\n");
      Terminate(ERR);
    }
  else
    {
      DebugPrintf (2, "\n\nBegin of Robot Data Section found. Good.");  
      // fflush(stdout);
    }
  
  DebugPrintf (2, "\n\nStarting to read robot calibration section\n\n");

  // Now we read in the speed calibration factor for all droids
  if ( (ValuePointer = strstr ( RobotPointer, MAXSPEED_CALIBRATOR_STRING )) == NULL )
    {
      DebugPrintf(1, "\nERROR! NO MAXSPEED CALIBRATOR ENTRY FOUND! TERMINATING!");
      Terminate(ERR);
    }
  else
    {
      ValuePointer += strlen ( MAXSPEED_CALIBRATOR_STRING );
      sscanf ( ValuePointer , "%lf" , &maxspeed_calibrator );
    }

  // Now we read in the acceleration calibration factor for all droids
  if ( (ValuePointer = strstr ( RobotPointer, ACCELERATION_CALIBRATOR_STRING )) == NULL )
    {
      DebugPrintf(1, "\nERROR! NO ACCELERATION CALIBRATOR ENTRY FOUND! TERMINATING!");
      Terminate(ERR);
    }
  else
    {
      ValuePointer += strlen ( ACCELERATION_CALIBRATOR_STRING );
      sscanf ( ValuePointer , "%lf" , &acceleration_calibrator );
    }

  // Now we read in the maxenergy calibration factor for all droids
  if ( (ValuePointer = strstr ( RobotPointer, MAXENERGY_CALIBRATOR_STRING )) == NULL )
    {
      DebugPrintf(1, "\nERROR! NO MAXENERGY CALIBRATOR ENTRY FOUND! TERMINATING!");
      Terminate(ERR);
    }
  else
    {
      ValuePointer += strlen ( MAXENERGY_CALIBRATOR_STRING );
      sscanf ( ValuePointer , "%lf" , &maxenergy_calibrator );
    }

  // Now we read in the energy_loss calibration factor for all droids
  if ( (ValuePointer = strstr ( RobotPointer, ENERGYLOSS_CALIBRATOR_STRING )) == NULL )
    {
      DebugPrintf(1, "\nERROR! NO ENERGYLOSS CALIBRATOR ENTRY FOUND! TERMINATING!");
      Terminate(ERR);
    }
  else
    {
      ValuePointer += strlen ( ENERGYLOSS_CALIBRATOR_STRING );
      sscanf ( ValuePointer , "%lf" , &energyloss_calibrator );
    }

  // Now we read in the aggression calibration factor for all droids
  if ( (ValuePointer = strstr ( RobotPointer, AGGRESSION_CALIBRATOR_STRING )) == NULL )
    {
      DebugPrintf(1, "\nERROR! NO AGGRESSION CALIBRATOR ENTRY FOUND! TERMINATING!");
      Terminate(ERR);
    }
  else
    {
      ValuePointer += strlen ( AGGRESSION_CALIBRATOR_STRING );
      sscanf ( ValuePointer , "%lf" , &aggression_calibrator );
    }

  // Now we read in the score calibration factor for all droids
  if ( (ValuePointer = strstr ( RobotPointer, SCORE_CALIBRATOR_STRING )) == NULL )
    {
      DebugPrintf(1, "\nERROR! NO SCORE CALIBRATOR ENTRY FOUND! TERMINATING!");
      Terminate(ERR);
    }
  else
    {
      ValuePointer += strlen ( SCORE_CALIBRATOR_STRING );
      sscanf ( ValuePointer , "%lf" , &score_calibrator );
    }


  DebugPrintf (2, "\n\nStarting to read Robot data...\n\n");
  //--------------------
  // At first, we must allocate memory for the droid specifications.
  // How much?  That depends on the number of droids defined in game.dat.
  // So we have to count those first.  ok.  lets do it.

  CountRobotsPointer=RobotPointer;
  Number_Of_Droid_Types=0;
  while ( ( CountRobotsPointer = strstr ( CountRobotsPointer, NEW_ROBOT_BEGIN_STRING)) != NULL)
    {
      CountRobotsPointer += strlen ( NEW_ROBOT_BEGIN_STRING );
      Number_Of_Droid_Types++;
    }
  // Not that we know how many robots are defined in game.dat, we can allocate
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
      if ( (ValuePointer = strstr ( RobotPointer, DROIDNAME_BEGIN_STRING )) == NULL )
	{
	  DebugPrintf(1, "\nERROR! NO DROIDNAME FOUND! TERMINATING!");
	  Terminate(ERR);
	}
      else
	{
	  ValuePointer += strlen (DROIDNAME_BEGIN_STRING);
	  StringLength = strstr (ValuePointer , "\n") - ValuePointer;
	  Druidmap[RobotIndex].druidname = MyMalloc ( StringLength + 1 );
	  strncpy ( (char*) Druidmap[RobotIndex].druidname , ValuePointer , StringLength );
	  Druidmap[RobotIndex].druidname[StringLength]=0;
	}

      // Now we read in the maximal speed this droid can go. 
      if ( (ValuePointer = strstr ( RobotPointer, MAXSPEED_BEGIN_STRING )) == NULL )
	{
	  DebugPrintf(1, "\nERROR! NO MAXSPEED ENTRY FOUND! TERMINATING!");
	  Terminate(ERR);
	}
      else
	{
	  ValuePointer += strlen ( MAXSPEED_BEGIN_STRING );
	  sscanf ( ValuePointer , "%lf" , &Druidmap[RobotIndex].maxspeed );
	}

      // Now we read in the class of this droid.
      if ( (ValuePointer = strstr ( RobotPointer, CLASS_BEGIN_STRING )) == NULL )
	{
	  DebugPrintf(1, "\nERROR! NO CLASS ENTRY FOUND! TERMINATING!");
	  Terminate(ERR);
	}
      else
	{
	  ValuePointer += strlen ( CLASS_BEGIN_STRING );
	  sscanf ( ValuePointer , "%d" , &Druidmap[RobotIndex].class );
	}

      // Now we read in the maximal acceleration this droid can go. 
      if ( (ValuePointer = strstr ( RobotPointer, ACCELERATION_BEGIN_STRING )) == NULL )
	{
	  DebugPrintf(1, "\nERROR! NO ACCELERATION ENTRY FOUND! TERMINATING!");
	  Terminate(ERR);
	}
      else
	{
	  ValuePointer += strlen ( ACCELERATION_BEGIN_STRING );
	  sscanf ( ValuePointer , "%lf" , &Druidmap[RobotIndex].accel );
	}

      // Now we read in the maximal energy this droid can store. 
      if ( (ValuePointer = strstr ( RobotPointer, MAXENERGY_BEGIN_STRING )) == NULL )
	{
	  DebugPrintf(1, "\nERROR! NO MAXENERGY ENTRY FOUND! TERMINATING!");
	  Terminate(ERR);
	}
      else
	{
	  ValuePointer += strlen ( MAXENERGY_BEGIN_STRING );
	  sscanf ( ValuePointer , "%lf" , &Druidmap[RobotIndex].maxenergy );
	}

      // Now we read in the lose_health rate.
      if ( (ValuePointer = strstr ( RobotPointer, LOSEHEALTH_BEGIN_STRING )) == NULL )
	{
	  DebugPrintf(1, "\nERROR! NO LOSE_HEALTH ENTRY FOUND! TERMINATING!");
	  Terminate(ERR);
	}
      else
	{
	  ValuePointer += strlen ( LOSEHEALTH_BEGIN_STRING );
	  sscanf ( ValuePointer , "%lf" , &Druidmap[RobotIndex].lose_health );
	}

      // Now we read in the class of this droid.
      if ( (ValuePointer = strstr ( RobotPointer, GUN_BEGIN_STRING )) == NULL )
	{
	  DebugPrintf(1, "\nERROR! NO GUN ENTRY FOUND! TERMINATING!");
	  Terminate(ERR);
	}
      else
	{
	  ValuePointer += strlen ( GUN_BEGIN_STRING );
	  sscanf ( ValuePointer , "%d" , &Druidmap[RobotIndex].gun );
	}

      // Now we read in the aggression rate of this droid.
      if ( (ValuePointer = strstr ( RobotPointer, AGGRESSION_BEGIN_STRING )) == NULL )
	{
	  DebugPrintf(1, "\nERROR! NO AGGRESSION ENTRY FOUND! TERMINATING!");
	  Terminate(ERR);
	}
      else
	{
	  ValuePointer += strlen ( AGGRESSION_BEGIN_STRING );
	  sscanf ( ValuePointer , "%d" , &Druidmap[RobotIndex].aggression );
	}

      // Now we read in the flash immunity of this droid.
      if ( (ValuePointer = strstr ( RobotPointer, FLASHIMMUNE_BEGIN_STRING )) == NULL )
	{
	  DebugPrintf(1, "\nERROR! NO FLASHIMMUNE ENTRY FOUND! TERMINATING!");
	  Terminate(ERR);
	}
      else
	{
	  ValuePointer += strlen ( FLASHIMMUNE_BEGIN_STRING );
	  sscanf ( ValuePointer , "%d" , &Druidmap[RobotIndex].flashimmune );
	}

      // Now we score to be had for destroying one droid of this type
      if ( (ValuePointer = strstr ( RobotPointer, SCORE_BEGIN_STRING )) == NULL )
	{
	  DebugPrintf(1, "\nERROR! NO SCORE ENTRY FOUND! TERMINATING!");
	  Terminate(ERR);
	}
      else
	{
	  ValuePointer += strlen ( SCORE_BEGIN_STRING );
	  sscanf ( ValuePointer , "%d" , &Druidmap[RobotIndex].score );
	  // printf("\nDroid score entry found!  It reads: %d" , Druidmap[RobotIndex].score );
	}

      // Now we read in the height of this droid of this type
      if ( (ValuePointer = strstr ( RobotPointer, HEIGHT_BEGIN_STRING ) ) == NULL )
	{
	  DebugPrintf(1, "\nERROR! NO HEIGHT ENTRY FOUND! TERMINATING!");
	  Terminate(ERR);
	}
      else
	{
	  ValuePointer += strlen ( HEIGHT_BEGIN_STRING );
	  sscanf ( ValuePointer , "%lf" , &Druidmap[RobotIndex].height );
	  // printf("\nDroid height entry found!  It reads: %f" , Druidmap[RobotIndex].height );
	}

      // Now we read in the weight of this droid type
      if ( (ValuePointer = strstr ( RobotPointer, WEIGHT_BEGIN_STRING )) == NULL )
	{
	  DebugPrintf(1, "\nERROR! NO WEIGHT ENTRY FOUND! TERMINATING!");
	  Terminate(ERR);
	}
      else
	{
	  ValuePointer += strlen ( WEIGHT_BEGIN_STRING );
	  sscanf ( ValuePointer , "%lf" , &Druidmap[RobotIndex].weight );
	  // printf( "\nDroid weight entry found!  It reads: %f" , Druidmap[RobotIndex].weight );
	}

      // Now we read in the drive of this droid of this type
      if ( (ValuePointer = strstr ( RobotPointer, DRIVE_BEGIN_STRING ) ) == NULL )
	{
	  DebugPrintf(1, "\nERROR! NO DRIVE ENTRY FOUND! TERMINATING!");
	  Terminate(ERR);
	}
      else
	{
	  ValuePointer += strlen ( DRIVE_BEGIN_STRING );
	  sscanf ( ValuePointer , "%d" , &Druidmap[RobotIndex].drive );
	  // printf("\nDroid drive entry found!  It reads: %d" , Druidmap[RobotIndex].drive );
	}

      // Now we read in the brain of this droid of this type
      if ( (ValuePointer = strstr ( RobotPointer, BRAIN_BEGIN_STRING ) ) == NULL )
	{
	  DebugPrintf(1, "\nERROR! NO BRAIN ENTRY FOUND! TERMINATING!");
	  Terminate(ERR);
	}
      else
	{
	  ValuePointer += strlen ( BRAIN_BEGIN_STRING );
	  sscanf ( ValuePointer , "%d" , &Druidmap[RobotIndex].brain );
	  // printf("\nDroid brain entry found!  It reads: %d" , Druidmap[RobotIndex].brain );
	}

      // Now we read in the sensor 1 of this droid type
      if ( (ValuePointer = strstr ( RobotPointer, SENSOR1_BEGIN_STRING ) ) == NULL )
	{
	  DebugPrintf(1, "\nERROR! NO SENSOR1 ENTRY FOUND! TERMINATING!");
	  Terminate(ERR);
	}
      else
	{
	  ValuePointer += strlen ( SENSOR1_BEGIN_STRING );
	  sscanf ( ValuePointer , "%d" , &Druidmap[RobotIndex].sensor1 );
	  // printf("\nDroid sensor 1 entry found!  It reads: %d" , Druidmap[RobotIndex].sensor1 );
	}

      // Now we read in the sensor 2 of this droid type
      if ( (ValuePointer = strstr ( RobotPointer, SENSOR2_BEGIN_STRING ) ) == NULL )
	{
	  DebugPrintf(1, "\nERROR! NO SENSOR2 ENTRY FOUND! TERMINATING!");
	  Terminate(ERR);
	}
      else
	{
	  ValuePointer += strlen ( SENSOR1_BEGIN_STRING );
	  sscanf ( ValuePointer , "%d" , &Druidmap[RobotIndex].sensor2 );
	  // printf("\nDroid sensor 2 entry found!  It reads: %d" , Druidmap[RobotIndex].sensor2 );
	}

      // Now we read in the sensor 3 of this droid type
      if ( (ValuePointer = strstr ( RobotPointer, SENSOR3_BEGIN_STRING ) ) == NULL )
	{
	  DebugPrintf (1,"\nERROR! NO SENSOR3 ENTRY FOUND! TERMINATING!");
	  Terminate(ERR);
	}
      else
	{
	  ValuePointer += strlen ( SENSOR3_BEGIN_STRING );
	  sscanf ( ValuePointer , "%d" , &Druidmap[RobotIndex].sensor3 );
	  // printf("\nDroid sensor 3 entry found!  It reads: %d" , Druidmap[RobotIndex].sensor3 );
	}

      // Now we read in the armament of this droid type
      if ( (ValuePointer = strstr ( RobotPointer, ARMAMENT_BEGIN_STRING ) ) == NULL )
	{
	  DebugPrintf (1, "\nERROR! NO ARMAMENT ENTRY FOUND! TERMINATING!");
	  Terminate(ERR);
	}
      else
	{
	  ValuePointer += strlen ( ARMAMENT_BEGIN_STRING );
	  sscanf ( ValuePointer , "%d" , &Druidmap[RobotIndex].armament );
	  // printf("\nDroid armament entry found!  It reads: %d" , Druidmap[RobotIndex].armament );
	}

      // Now we read in the AdvancedFighing flag of this droid type
      if ( (ValuePointer = strstr ( RobotPointer, ADVANCED_FIGHTING_BEGIN_STRING ) ) == NULL )
	{
	  DebugPrintf (1, "\nERROR! NO ADVANCED FIGHTING ENTRY FOUND! TERMINATING!");
	  Terminate(ERR);
	}
      else
	{
	  ValuePointer += strlen ( ADVANCED_FIGHTING_BEGIN_STRING );
	  sscanf ( ValuePointer , "%d" , &Druidmap[RobotIndex].AdvancedBehaviour );
	  // printf("\nDroid AdvancedFighting entry found!  It reads: %d" , Druidmap[RobotIndex].AdvancedFighting );
	}

      // Now we read in the notes concerning this droid.  We consider as notes all the rest of the
      // line after the NOTES_BEGIN_STRING until the "\n" is found.
      if ( (ValuePointer = strstr ( RobotPointer, NOTES_BEGIN_STRING )) == NULL )
	{
	  DebugPrintf (1, "\nERROR! NO NOTES ENTRY FOUND! TERMINATING!");
	  Terminate(ERR);
	}
      else
	{
	  ValuePointer += strlen (NOTES_BEGIN_STRING);
	  StringLength = strstr (ValuePointer , "\n") - ValuePointer;
	  Druidmap[RobotIndex].notes = MyMalloc ( StringLength + 1 );
	  strncpy ( (char*) Druidmap[RobotIndex].notes , ValuePointer , StringLength );
	  Druidmap[RobotIndex].notes[StringLength]=0;
	  // printf("\nNotes concerning the droid found!  They read: %s" , Druidmap[RobotIndex].notes );
	}



      // Now we're potentially ready to process the next droid.  Therefore we proceed to
      // the next number in the Droidmap array.
      RobotIndex++;
    }
  

  DebugPrintf (2, "\n\nThat must have been the last robot.  We're done reading the robot data.");
  DebugPrintf (2, "\n\nApplying the calibration factors to all droids...");

  for ( i=0; i< Number_Of_Droid_Types ; i++ ) 
    {
      Druidmap[i].maxspeed *= maxspeed_calibrator;
      Druidmap[i].accel *= acceleration_calibrator;
      Druidmap[i].maxenergy *= maxenergy_calibrator;
      Druidmap[i].lose_health *= energyloss_calibrator;
      Druidmap[i].aggression *= aggression_calibrator;
      Druidmap[i].score *= score_calibrator;
    }


  Druidmap[ DRUID302 ].CallForHelpAfterSufferingHit = TRUE;


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
  char *ShipnamePointer;
  char *ShipOnPointer;
  char *ShipOffPointer;
  char *LiftnamePointer;
  char *CrewnamePointer;
  char *GameDataNamePointer;
  char *EndTitlePointer;
  char *StartPointPointer;
  char *MissionTargetPointer;
  char *NextMissionNamePointer;
  char Shipname[2000];
  char Liftname[2000];
  char Crewname[2000];
  char GameDataName[2000];
  int ShipnameLength;
  int ShipOnLength;
  int ShipOffLength;
  int CrewnameLength;
  int LiftnameLength;
  int GameDataNameLength;
  int EndTitleLength;
  int NextMissionNameLength;
  int NumberOfStartPoints=0;
  int RealStartPoint=0;
  int StartingLevel=0;
  int StartingXPos=0;
  int StartingYPos=0;

#define END_OF_MISSION_DATA_STRING "*** End of Mission File ***"
#define MISSION_BRIEFING_BEGIN_STRING "** Start of Mission Briefing Text Section **"
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
#define NEXT_MISSION_NAME_STRING "After completing this mission, load mission : "




  DebugPrintf (2, "\nvoid InitNewMission( char *MissionName ): real function call confirmed...");
  DebugPrintf (1, "\nA new mission is being initialized from file %s.\n" , MissionName );

  //--------------------
  //At first we do the things that must be done for all
  //missions, regardless of mission file given
  Activate_Conservative_Frame_Computation();
  LastBlastHit = 0;
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
  KillQueue (); // This has NO meaning right now...
  InsertMessage (" Game on!  Good Luck,,."); // this also has NO meaning right now

  /* Delete all bullets and blasts */
  for (i = 0; i < MAXBULLETS; i++)
    {
      AllBullets[i].type = OUT;
      AllBullets[i].mine = FALSE;
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

  /* Read the whole mission data to memory */

  fpath = find_file (MissionName, MAP_DIR, FALSE);

  MainMissionPointer = ReadAndMallocAndTerminateFile( fpath , END_OF_MISSION_DATA_STRING ) ;

  //--------------------
  //Now the mission file is read into memory
  //That means we can start to decode the details given
  //in the body of the mission file.  We start with 
  //doing the briefing things...

  // Now we search for the beginning of the WHOLE event section within the mission file
  if ( ( EventSectionPointer = strstr ( MainMissionPointer, EVENT_SECTION_BEGIN_STRING )) == NULL )
    {
      DebugPrintf (1, "\nERROR! NO MISSION EVENT SECTION BEGIN STRING FOUND! TERMINATING!");
      Terminate(ERR);
    }
  else
    {
      EventSectionPointer += strlen ( EVENT_SECTION_BEGIN_STRING ) +1;
      DebugPrintf (1, "\nMission Briefing begin BIG section found!");
    }

  /* Title and Explanation of controls and such... */
  Get_Mission_Events ( EventSectionPointer );
  DebugPrintf (2, "\nvoid InitNewMission(void): The title signaton has been successfully displayed...:");

  //--------------------
  // First we extract the game physics file name from the
  // mission file and load the game data.
  //

  // Now we search for the beginning of the mission briefing big section NOT subsection
  if ( ( BriefingSectionPointer = strstr ( MainMissionPointer, MISSION_BRIEFING_BEGIN_STRING )) == NULL )
    {
      DebugPrintf (1, "\nERROR! NO MISSION BRIEFING BEGIN STRING FOUND! TERMINATING!");
      Terminate(ERR);
    }
  else
    {
      BriefingSectionPointer += strlen ( MISSION_BRIEFING_BEGIN_STRING ) +1;
      DebugPrintf (2, "\nMission Briefing begin BIG section found!");
    }

  /* Title and Explanation of controls and such... */
  Title ( BriefingSectionPointer );
  DebugPrintf (2, "\nvoid InitNewMission(void): The title signaton has been successfully displayed...:");

  //--------------------
  // First we extract the game physics file name from the
  // mission file and load the game data.
  //
  if ( (GameDataNamePointer = strstr ( MainMissionPointer, GAMEDATANAME_INDICATION_STRING )) == NULL )
    {
      DebugPrintf (1, "\nERROR! NO GAME DATA FILENAME FOUND! TERMINATING!");
      Terminate(ERR);
    }
  else
    {
      GameDataNamePointer += strlen ( GAMEDATANAME_INDICATION_STRING );
      
      GameDataNameLength = strstr ( GameDataNamePointer , "\n") - GameDataNamePointer;
      strncpy( GameDataName , GameDataNamePointer , GameDataNameLength );
      GameDataName[ GameDataNameLength ] = 0;
      DebugPrintf (1, "\nGame data filename found!  It reads: %s" , GameDataName );
    }
  
  Init_Game_Data ( GameDataName );

  //--------------------
  // Now its time to get the shipname from the mission file and
  // read the ship file into the right memory structures
  //
  if ( (ShipnamePointer = strstr ( MainMissionPointer, SHIPNAME_INDICATION_STRING )) == NULL )
    {
      DebugPrintf (1, "\nERROR! NO SHIPNAME FOUND! TERMINATING!");
      Terminate(ERR);
    }
  else
    {
      ShipnamePointer += strlen ( SHIPNAME_INDICATION_STRING );
      
      ShipnameLength = strstr (ShipnamePointer , "\n") - ShipnamePointer;
      strncpy( Shipname , ShipnamePointer , ShipnameLength );
      Shipname[ ShipnameLength ] = 0;
      
      
      DebugPrintf (1, "\nShipname found!  It reads: %s" , Shipname );
    }
  
  if ( LoadShip ( Shipname ) == ERR )
    {
      DebugPrintf (1, "Error in LoadShip\n");
      Terminate (ERR);
    }

  //--------------------
  // Now its time to get the elevator file name from the mission file and
  // read the elevator file into the right memory structures
  //
  if ( (LiftnamePointer = strstr ( MainMissionPointer, ELEVATORNAME_INDICATION_STRING )) == NULL )
    {
      DebugPrintf (1, "\nERROR! NO ELEVATORNAME FOUND! TERMINATING!");
      DebugPrintf (1, "Lift indication string was: %s." , ELEVATORNAME_INDICATION_STRING );
      Terminate(ERR);
    }
  else
    {
      LiftnamePointer += strlen ( ELEVATORNAME_INDICATION_STRING );
      
      LiftnameLength = strstr (LiftnamePointer , "\n") - LiftnamePointer;
      strncpy( Liftname , LiftnamePointer , LiftnameLength );
      Liftname[ LiftnameLength ] = 0;
      
      DebugPrintf (1, "\nLift file name found!  It reads: %s" , Liftname );
    }
  
  /* Get the elevator connections */
  if (GetLiftConnections ( Liftname ) == ERR)
    {
      DebugPrintf (1, "\nError in GetLiftConnections ");
      Terminate (ERR);
    }

  //--------------------
  // Now its time to get the lifts on/off picturec file name from the mission file and
  // assemble an appropriate crew out of it
  //
  if ( ( ShipOnPointer = strstr ( MainMissionPointer, LIFTS_ON_INDICATION_STRING )) == NULL )
    {
      DebugPrintf (1, "\nERROR! NO LIFTS ON FILENAME FOUND! TERMINATING!");
      Terminate(ERR);
    }
  else
    {
      ship_on_filename = MyMalloc(2000);
      ShipOnPointer += strlen ( LIFTS_ON_INDICATION_STRING );
      
      ShipOnLength = strstr ( ShipOnPointer , "\n") - ShipOnPointer;
      strncpy (ship_on_filename , ShipOnPointer , ShipOnLength );
      ship_on_filename[ ShipOnLength ] = 0;
      
      DebugPrintf (1, "\nShipOn file name found!  It reads: %s" , ship_on_filename );
    }
  if ( ( ShipOffPointer = strstr ( MainMissionPointer, LIFTS_OFF_INDICATION_STRING )) == NULL )
    {
      DebugPrintf (1, "\nERROR! NO LIFTS OFF FILENAME FOUND! TERMINATING!");
      Terminate(ERR);
    }
  else
    {
      ship_off_filename = MyMalloc(2000);
      ShipOffPointer += strlen ( LIFTS_OFF_INDICATION_STRING );
      
      ShipOffLength = strstr ( ShipOffPointer , "\n") - ShipOffPointer;
      strncpy( ship_off_filename , ShipOffPointer , ShipOffLength );
      ship_off_filename[ ShipOffLength ] = 0;
      
      DebugPrintf (1, "\nShipOff file name found!  It reads: %s" , ship_off_filename );
    }

  // getchar();

  //--------------------
  // Now its time to get the crew file name from the mission file and
  // assemble an appropriate crew out of it
  //
  if ( (CrewnamePointer = strstr ( MainMissionPointer, CREWNAME_INDICATION_STRING )) == NULL )
    {
      DebugPrintf (1, "\nERROR! NO CREWNAME FOUND! TERMINATING!");
      Terminate(ERR);
    }
  else
    {
      CrewnamePointer += strlen ( CREWNAME_INDICATION_STRING );
      
      CrewnameLength = strstr (CrewnamePointer , "\n") - CrewnamePointer;
      strncpy( Crewname , CrewnamePointer , CrewnameLength );
      Crewname[ CrewnameLength ] = 0;
      
      DebugPrintf (1, "\nCrew file name found!  It reads: %s" , Crewname );
    }
  
  /* initialize enemys according to crew file */
  // WARNING!! THIS REQUIRES THE game.dat FILE TO BE READ ALREADY, BECAUSE
  // ROBOT SPECIFICATIONS ARE ALREADY REQUIRED HERE!!!!!
  //

  if (GetCrew ( Crewname ) == ERR)
    {
      DebugPrintf (1, "\nInitNewGame(): ERROR: Initialization of enemys failed...");
      Terminate (-1);
    }

  //--------------------
  // Now its time to get the shipname from the mission file and
  // read the ship file into the right memory structures
  //
  if ( (EndTitlePointer = strstr ( MainMissionPointer, MISSION_ENDTITLE_BEGIN_STRING )) == NULL )
    {
      DebugPrintf (1, "\nERROR! NO END TITLE SECTION BEGIN STRING FOUND! TERMINATING!");
      Terminate(ERR);
    }
  else
    {
      EndTitlePointer += strlen ( MISSION_ENDTITLE_BEGIN_STRING ) + 1;
      EndTitleLength = strstr ( EndTitlePointer , MISSION_ENDTITLE_END_STRING ) - EndTitlePointer;
      DebriefingText = MyMalloc ( EndTitleLength +10 );
      strncpy( DebriefingText , EndTitlePointer , EndTitleLength +1 );
      DebriefingText[EndTitleLength] = 0;
      
      // DebugPrintf (1, "\nEnd title string found!  It reads: %s" , DebriefingText );
    }
  
  //--------------------
  // Now we read all the possible starting points for the
  // current mission file, so that we know where to place the
  // influencer at the beginning of the mission.
  StartPointPointer=MainMissionPointer;
  while ( ( StartPointPointer = strstr ( StartPointPointer, MISSION_START_POINT_STRING )) != NULL )
    {
      NumberOfStartPoints++;
      StartPointPointer+=strlen( MISSION_START_POINT_STRING );
      DebugPrintf (2, "\nFound another starting point entry!");
    }
  DebugPrintf (1, "\nFound %d different starting points for the mission in the mission file.", NumberOfStartPoints );

  RealStartPoint = MyRandom ( NumberOfStartPoints -1 ) + 1;
  // DebugPrintf (1, "\nRealStartPoint: %d." , RealStartPoint);

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
  // DebugPrintf (1, "\nFinal starting position: Level=%d XPos=%d YPos=%d." , StartingLevel, StartingXPos, StartingYPos );
  
  if ( NumberOfStartPoints == 0 )
    {
      DebugPrintf (1, "\n\nERROR! NOT EVEN ONE SINGLE STARTING POINT ENTRY FOUND!  TERMINATING!");
      Terminate(ERR);
    }

  // Now that we know how many different starting points there are, we can randomly select
  // one of them


  if ( ( StartPointPointer = strstr ( MainMissionPointer, MISSION_START_POINT_STRING )) == NULL )
    {
      DebugPrintf (1, "\nERROR! NO MISSION START POINT STRING FOUND! TERMINATING!");
      Terminate(ERR);
    }
  else
    {
      StartPointPointer += strlen ( MISSION_START_POINT_STRING ) +1;
      DebugPrintf (2, "\nMission Start Point string found!");
    }

  //--------------------
  // Now we read in the mission targets for this mission
  // Several different targets may be specified simultaneously
  //
  if ( ( MissionTargetPointer = strstr ( MainMissionPointer, MISSION_TARGET_KILL_ALL_STRING )) == NULL )
    {
      DebugPrintf (1, "\nERROR! NO MISSION TARGET KILLALL ENTRY FOUND! TERMINATING!");
      Terminate(ERR);
    }
  else
    {
      MissionTargetPointer += strlen ( MISSION_TARGET_KILL_ALL_STRING );
      sscanf ( MissionTargetPointer , "%d" , &Me.mission.KillAll );
      // DebugPrintf (1, "\nMission target killall entry found!  It reads: %d" , Me.mission.KillAll );
    }

  if ( ( MissionTargetPointer = strstr ( MainMissionPointer, MISSION_TARGET_KILL_CLASS_STRING )) == NULL )
    {
      DebugPrintf (1, "\nERROR! NO MISSION TARGET KILLALL CLASS ENTRY FOUND! TERMINATING!");
      Terminate(ERR);
    }
  else
    {
      MissionTargetPointer += strlen ( MISSION_TARGET_KILL_CLASS_STRING );
      sscanf ( MissionTargetPointer , "%d" , &Me.mission.KillClass );
      // DebugPrintf (1, "\nMission target killclass entry found!  It reads: %d" , Me.mission.KillClass );
    }

  if ( ( MissionTargetPointer = strstr ( MainMissionPointer, MISSION_TARGET_KILL_ONE_STRING )) == NULL )
    {
      DebugPrintf (1, "\nERROR! NO MISSION TARGET KILLONE ENTRY FOUND! TERMINATING!");
      Terminate(ERR);
    }
  else
    {
      MissionTargetPointer += strlen ( MISSION_TARGET_KILL_ONE_STRING );
      sscanf ( MissionTargetPointer , "%d" , &Me.mission.KillOne );
      // DebugPrintf (1, "\nMission target killone entry found!  It reads: %d" , Me.mission.KillOne );
    }

  if ( ( MissionTargetPointer = strstr ( MainMissionPointer, MISSION_TARGET_MUST_BE_CLASS_STRING )) == NULL )
    {
      DebugPrintf (1, "\nERROR! NO MISSION TARGET MUST BE CLASS ENTRY FOUND! TERMINATING!");
      Terminate(ERR);
    }
  else
    {
      MissionTargetPointer += strlen ( MISSION_TARGET_MUST_BE_CLASS_STRING );
      sscanf ( MissionTargetPointer , "%d" , &Me.mission.MustBeClass );
      // DebugPrintf (1, "\nMission target MustBeClass entry found!  It reads: %d" , Me.mission.MustBeClass );
    }

  if ( ( MissionTargetPointer = strstr ( MainMissionPointer, MISSION_TARGET_MUST_BE_TYPE_STRING )) == NULL )
    {
      DebugPrintf (1, "\nERROR! NO MISSION TARGET MUST BE TYPE ENTRY FOUND! TERMINATING!");
      Terminate(ERR);
    }
  else
    {
      MissionTargetPointer += strlen ( MISSION_TARGET_MUST_BE_TYPE_STRING );
      sscanf ( MissionTargetPointer , "%d" , &Me.mission.MustBeType );
      // DebugPrintf (1, "\nMission target MustBeType entry found!  It reads: %d" , Me.mission.MustBeType );
    }

  if ( ( MissionTargetPointer = strstr ( MainMissionPointer, MISSION_TARGET_MUST_BE_ONE_STRING )) == NULL )
    {
      DebugPrintf (1, "\nERROR! NO MISSION TARGET MUST BE ONE ENTRY FOUND! TERMINATING!");
      Terminate(ERR);
    }
  else
    {
      MissionTargetPointer += strlen ( MISSION_TARGET_MUST_BE_ONE_STRING );
      sscanf ( MissionTargetPointer , "%d" , &Me.mission.MustBeOne );
      // DebugPrintf (1, "\nMission target MustBeOne entry found!  It reads: %d" , Me.mission.MustBeOne );
    }

  if ( ( MissionTargetPointer = strstr ( MainMissionPointer, MISSION_TARGET_MUST_REACH_LEVEL_STRING )) == NULL )
    {
      DebugPrintf (1, "\nERROR! NO MISSION TARGET MUST REACH LEVEL ENTRY FOUND! TERMINATING!");
      Terminate(ERR);
    }
  else
    {
      MissionTargetPointer += strlen ( MISSION_TARGET_MUST_REACH_LEVEL_STRING );
      sscanf ( MissionTargetPointer , "%d" , &Me.mission.MustReachLevel );
      // DebugPrintf (1, "\nMission target MustReachLevel entry found!  It reads: %d" , Me.mission.MustReachLevel );
    }

  if ( ( MissionTargetPointer = strstr ( MainMissionPointer, MISSION_TARGET_MUST_REACH_POINT_X_STRING )) == NULL )
    {
      DebugPrintf (1, "\nERROR! NO MISSION TARGET MUST REACH X ENTRY FOUND! TERMINATING!");
      Terminate(ERR);
    }
  else
    {
      MissionTargetPointer += strlen ( MISSION_TARGET_MUST_REACH_POINT_X_STRING );
      sscanf ( MissionTargetPointer , "%d" , &Me.mission.MustReachPoint.x );
      // DebugPrintf (1, "\nMission target MustReachPoint.x entry found!  It reads: %d" , Me.mission.MustReachPoint.x );
    }

  if ( ( MissionTargetPointer = strstr ( MainMissionPointer, MISSION_TARGET_MUST_REACH_POINT_Y_STRING )) == NULL )
    {
      DebugPrintf (1, "\nERROR! NO MISSION TARGET MUST REACH Y ENTRY FOUND! TERMINATING!");
      Terminate(ERR);
    }
  else
    {
      MissionTargetPointer += strlen ( MISSION_TARGET_MUST_REACH_POINT_Y_STRING );
      sscanf ( MissionTargetPointer , "%d" , &Me.mission.MustReachPoint.y );
      // DebugPrintf (1, "\nMission target MustReachPoint.y entry found!  It reads: %d" , Me.mission.MustReachPoint.y );
    }

  if ( ( MissionTargetPointer = strstr ( MainMissionPointer, MISSION_TARGET_MUST_LIVE_TIME_STRING )) == NULL )
    {
      DebugPrintf (1, "\nERROR! NO MISSION TARGET MUST LIVE TIME ENTRY FOUND! TERMINATING!");
      Terminate(ERR);
    }
  else
    {
      MissionTargetPointer += strlen ( MISSION_TARGET_MUST_LIVE_TIME_STRING );
      sscanf ( MissionTargetPointer , "%lf" , &Me.mission.MustLiveTime );
      // DebugPrintf (1, "\nMission target MustLiveTime entry found!  It reads: %f" , Me.mission.MustLiveTime );
    }

  //--------------------
  // After the mission targets have been successfully loaded now,
  // we need to add a pointer to the next mission, so that we will later
  // now which mission to load after this mission has been completed.
  //
  if ( ( NextMissionNamePointer = strstr ( MainMissionPointer, NEXT_MISSION_NAME_STRING )) == NULL )
    {
      DebugPrintf (1, "\nERROR! NO MISSION NEXT MISSION NAME ENTRY FOUND! TERMINATING!");
      Terminate(ERR);
    }
  else
    {
      NextMissionNamePointer += strlen ( NEXT_MISSION_NAME_STRING ) ;
      NextMissionNameLength = strstr ( NextMissionNamePointer , "\n" ) - NextMissionNamePointer;
      NextMissionName = MyMalloc ( NextMissionNameLength +10 );
      strncpy( NextMissionName , NextMissionNamePointer , NextMissionNameLength +1 );
      NextMissionName[NextMissionNameLength] = 0;
      // DebugPrintf (1, "\nNext mission name found!  It reads: %s" , NextMissionName );
    }
  
  /* Reactivate the light on alle Levels, that might have been dark */
  for (i = 0; i < curShip.num_levels; i++)
    curShip.AllLevels[i]->empty = FALSE;
  DebugPrintf (2, "\nvoid InitNewMission( ... ): All levels have been set to 'active'...");

  /* Den Banner fuer das Spiel anzeigen */
  ClearGraphMem();
  DisplayBanner (NULL, NULL,  BANNER_FORCE_UPDATE );

  SetTextColor (FONT_WHITE, FONT_RED);
  InitBars = TRUE;

  Switch_Background_Music_To (COMBAT_BACKGROUND_MUSIC_SOUND);

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

  /* Set colors of current level NOTE: THIS REQUIRES CurLevel TO BE INITIALIZED */
  SetLevelColor (CurLevel->color); 

  /* set correct Influ color */
  // SetPalCol (INFLUENCEFARBWERT, Mobilecolor.rot, Mobilecolor.gruen,
  // Mobilecolor.blau);

  ShuffleEnemys(); // NOTE: THIS REQUIRES CurLevel TO BE INITIALIZED

  DebugPrintf (1, "done."); // this matches the printf at the beginning of this function
  fflush(stdout);
  
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
  struct timeval timestamp;

  Bulletmap=NULL;  // That will cause the memory to be allocated later

  Overall_Average=0.041;
  SkipAFewFrames = 0;
  Me.TextVisibleTime = 0;
  Me.TextToBeDisplayed = "Hello, I'm 001.";

  GameConfig.WantedTextVisibleTime = 3;
  GameConfig.Draw_Framerate=FALSE;
  GameConfig.All_Texts_Switch=TRUE;
  GameConfig.Enemy_Hit_Text=FALSE;
  GameConfig.Enemy_Bump_Text=TRUE;
  GameConfig.Enemy_Aim_Text=TRUE;
  GameConfig.Influencer_Refresh_Text=FALSE;
  GameConfig.Influencer_Blast_Text=TRUE;
  GameConfig.Theme_SubPath="lanzz_theme/";

  Init_Video ();

  Init_Audio ();
  
  Init_Joy ();

  Init_Game_Data("game.dat");  // load the default ruleset. This can be */
			       // overwritten from the mission file.

  // The default should be, that no rescaling of the
  // combat window at all is done.
  CurrentCombatScaleFactor = 1;

  /* 
   * Initialise random-number generator in order to make 
   * level-start etc really different at each program start
   */
  gettimeofday(&timestamp, NULL);
  srand((unsigned int) timestamp.tv_sec); /* yes, we convert long->int here! */

  /* initialize the high score values */
  /* 
   * this really should be read from disk here, 
   * but for the moment we just start from zero 
   * each time
   */
  highscores = NULL;

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

  /* Init the Takeover- Game */
  InitTakeover ();

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
  int ScrollEndLine = USERFENSTERPOSY;	/* Endpunkt des Scrollens */
  char* NextSubsectionStartPointer;
  char* PreparedBriefingText;
  char* TerminationPointer;
  char* TitlePictureNamePointer;
  char TitlePictureName[5000];
  int ThisTextLength;
#define BRIEFING_TITLE_PICTURE_STRING "The title picture in the graphics subdirectory for this mission is : "
#define NEXT_BRIEFING_SUBSECTION_START_STRING "* New Mission Briefing Text Subsection *"
#define END_OF_BRIEFING_SUBSECTION_STRING "* End of Mission Briefing Text Subsection *"

  // STRANGE!! This command will be silently ignored by SDL?
  // WHY?? DONT KNOW!!!
  // Play_Sound ( CLASSICAL_BEEP_BEEP_BACKGROUND_MUSIC );
  // Play_Sound ( CLASSICAL_BEEP_BEEP_BACKGROUND_MUSIC );
  Switch_Background_Music_To ( CLASSICAL_BEEP_BEEP_BACKGROUND_MUSIC );
  // Switch_Background_Music_To ( COMBAT_BACKGROUND_MUSIC_SOUND );

  if ( ( TitlePictureNamePointer = strstr ( MissionBriefingPointer , BRIEFING_TITLE_PICTURE_STRING)) != NULL)
    {
      TitlePictureNamePointer += strlen ( BRIEFING_TITLE_PICTURE_STRING );
      ThisTextLength = strstr(TitlePictureNamePointer , "\n" ) - TitlePictureNamePointer ;
      strncpy ( TitlePictureName , TitlePictureNamePointer , ThisTextLength);
      TitlePictureName[ ThisTextLength ] = 0;
      DebugPrintf( 1 , "\nvoid Title(...): The briefing picture name found is : %s." , TitlePictureName );
    }
  else
    {
      DebugPrintf ( 0 , "\n\nvoid Title(...): Title picture specification string not found....Terminating..." );
      Terminate( ERR );
    }

  SDL_SetClipRect ( ne_screen, NULL );
  DisplayImage ( find_file(TitlePictureName, GRAPHICS_DIR, FALSE) );
  SDL_Flip (ne_screen);

  Me.status=BRIEFING;

  // ClearGraphMem ();
  // DisplayBanner (NULL, NULL,  BANNER_FORCE_UPDATE ); 

  // SetTextColor (FONT_BLACK, FONT_RED);

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
  SDL_Flip( ne_screen );

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
  int ScrollEndLine = USERFENSTERPOSY;	/* Endpunkt des Scrollens */

  DebugPrintf (2, "\nvoid EndTitle(void): real function call confirmed...:");

  Switch_Background_Music_To (CLASSICAL_BEEP_BEEP_BACKGROUND_MUSIC);

  DisplayBanner (NULL, NULL,  BANNER_FORCE_UPDATE );

  SetTextColor (FONT_BLACK, FONT_RED);

  // SetCurrentFont( FPS_Display_BFont );
  SetCurrentFont( Para_BFont );

  ScrollText ( DebriefingText , SCROLLSTARTX, SCROLLSTARTY, ScrollEndLine , NE_TITLE_PIC_FILE );

  while ( SpacePressed() );

} /* EndTitle() */

/*@Function============================================================
@Desc: Diese Funktion Sprengt den Influencer und beendet das Programm

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
ThouArtDefeated (void)
{
  int j;
  int now;

  DebugPrintf (2, "\nvoid ThouArtDefeated(void): Real function call confirmed.");
  Me.status = TERMINATED;
  ThouArtDefeatedSound ();
  ExplodeInfluencer ();

  now=SDL_GetTicks();

  while ( SDL_GetTicks()-now < 1000 * WAIT_AFTER_KILLED )
    {
      Assemble_Combat_Picture ( DO_SCREEN_UPDATE );
      DisplayBanner (NULL, NULL,  0 );
      ExplodeBlasts ();
      MoveBullets ();
      MoveEnemys ();

      for (j = 0; j < MAXBULLETS; j++)
	CheckBulletCollisions (j);
      RotateBulletColor ();
    }

  Debriefing ();

  GameOver = TRUE;

  DebugPrintf (2, "\nvoid ThouArtDefeated(void): Usual end of function reached.");
  DebugPrintf (1, "\n\n DefeatedDone \n\n");
}				// void ThouArtDefeated(void)

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
 * Mission debriefing.  
 * Mainly managing of highscore entries 
 *
 *----------------------------------------------------------------------*/
void
Debriefing (void)
{
  char *tmp_name;
  Hall_entry new, tmp, last;
  //  int DebriefColor;
  int count;
  //  BFont_Info *prev_font;

  if (RealScore <= 0)  /* don't even bother.. */
    return;

  //  DebriefColor = FONT_WHITE;
  Me.status = DEBRIEFING;
  //  SetUserfenster (DebriefColor);	// KON_BG_COLOR

  count = 0;
  if ( (tmp = highscores) != NULL)
    {
      count = 1;
      while (tmp->next) { count++; tmp = tmp->next;}  /* tmp now points to lowest! */
      last = tmp; /* remember this one */
    }


  if ( (count == MAX_HIGHSCORES) && (RealScore <= last->score) )
    return; /* looser! ;) */
      
  /* else: prepare a new entry */
  new = MyMalloc (sizeof(hall_entry));
  new->score = RealScore;
  new->next = new->prev = NULL;
  //  prev_font = CurrentFont;
  //  SetCurrentFont (Highscore_BFont);
  printf_SDL (ne_screen, User_Rect.x, User_Rect.y, "Great Score !\n");
  printf_SDL (ne_screen, -1, -1, "Enter your name: ");
  tmp_name = GetString (MAX_NAME_LEN, 2);
  strcpy (new->name, tmp_name);
  free (tmp_name);
  //  SetCurrentFont (prev_font);

  if (!highscores)  /* hey, no previous entries! */
    highscores = new;
  else if (RealScore <= last->score) /* you're the last */
    {
      count ++;
      last->next = new;
      new->prev = last;
    }
  else   /* link in the new entry inside existing list */
    {
      count ++;
      tmp = last;  /* work your way back up from last enty */
      while ( tmp->prev && (RealScore > ((Hall_entry)(tmp->prev))->score) )
	tmp = tmp->prev;

      /* tmp now points to the entry to be pushed down */
      ((Hall_entry)tmp->prev)->next = new;
      new->prev = tmp->prev;
      new->next = tmp;
      tmp->prev = new;
    }

  /* now check the length of our new highscore list.
   * if longer than MAX_HIGHSCORES */

  if ( count > MAX_HIGHSCORES ) /* the last one drops out */
    {
      ((Hall_entry)(last->prev))->next = NULL;
      free (last);
    }
  
  return;

} /* Debriefing() */

/*----------------------------------------------------------------------
 * This function checks, if the influencer has succeeded in his given 
 * mission.  If not it returns, if yes the EndTitle/Debriefing is
 * started.
 ----------------------------------------------------------------------*/
void 
CheckIfMissionIsComplete (void)
{
  int Robot_Counter;

  if ( Me.mission.KillOne != (-1) )
    {
      for ( Robot_Counter=0 ; Robot_Counter < MAX_ENEMYS_ON_SHIP ; Robot_Counter++ )
	{
	  if ( ( AllEnemys[Robot_Counter].energy > 0 ) && 
	       ( AllEnemys[Robot_Counter].Status != OUT ) && 
	       ( AllEnemys[Robot_Counter].Marker == Me.mission.KillOne ) )
	    {
	      DebugPrintf ( 2, "\nOne of the marked droids is still alive...");
	      // fflush(stdout);
	      return;
	    }
	}
    }

  if ( Me.mission.KillAll != (-1) )
    {
      for ( Robot_Counter=0 ; Robot_Counter < MAX_ENEMYS_ON_SHIP ; Robot_Counter++ )
	{
	  if ( AllEnemys[Robot_Counter].energy > 0 ) 
	    {
	      DebugPrintf (2, "\nThere are some robots still alive, and you should kill them all...");
	      // fflush(stdout);
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
	      DebugPrintf (2, "\nOne of that class is still alive: Nr=%d Lev=%d X=%f Y=%f." , 
			   Robot_Counter , AllEnemys[Robot_Counter].levelnum , 
			   AllEnemys[Robot_Counter].pos.x , AllEnemys[Robot_Counter].pos.y );
	      return;
	    }
	}
    }

  if ( Me.mission.MustBeClass != (-1) )
    {
      // DebugPrintf (1, "\nMe.type is now: %d.", Me.type );
      if ( Druidmap[Me.type].class != Me.mission.MustBeClass ) 
	{
	  // DebugPrintf (2, "\nMe.class does not match...");
	  return;
	}
    }

  if ( Me.mission.MustBeType != (-1) )
    {
      // DebugPrintf (1, "\nMe.type is now: %d.", Me.type );
      if ( Me.type != Me.mission.MustBeType ) 
	{
	  // DebugPrintf (1, "\nMe.type does not match...");
	  return;
	}
    }

  if ( Me.mission.MustReachLevel != (-1) )
    {
      if ( CurLevel->levelnum != Me.mission.MustReachLevel ) 
	{
	  // DebugPrintf (1, "\nLevel number does not match...");
	  return;
	}
    }

  if ( Me.mission.MustReachPoint.x != (-1) )
    {
      if ( Me.pos.x != Me.mission.MustReachPoint.x ) 
	{
	  // DebugPrintf (1, "\nX coordinate does not match...");
	  return;
	}
    }

  if ( Me.mission.MustReachPoint.y != (-1) )
    {
      if ( Me.pos.y != Me.mission.MustReachPoint.y ) 
	{
	  // DebugPrintf (1, "\nY coordinate does not match..."); 
	  return;
	}
    }

  if ( Me.mission.MustLiveTime != (-1) )
    {
      if ( Me.MissionTimeElapsed < Me.mission.MustLiveTime ) 
	{
	  // DebugPrintf (1, "\nTime Limit not yet reached...");
	  return;
	}
    }

  if ( Me.mission.MustBeOne != (-1) )
    {
      if ( Me.Marker != Me.mission.MustBeOne ) 
	{
	  // DebugPrintf (1, "\nYou're not yet one of the marked ones...");
	  // fflush(stdout);
	  return;
	}
    }


  EndTitle();
  // GameOver=TRUE;

  InitNewMission ( NextMissionName);
  
} // void CheckIfMissionIsComplete



#undef _init_c



