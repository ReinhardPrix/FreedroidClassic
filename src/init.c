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

/* ----------------------------------------------------------------------
 * (Not all of the) Initialisation routines for FreeDroid.
 * ---------------------------------------------------------------------- */

#define _init_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"
#include "ship.h"
#include "fenv.h"

#include "getopt.h"

void Init_Game_Data( char* Datafilename );
void Get_Bullet_Data ( char* DataPointer );
extern int feenableexcept (int TheExceptionFlags );

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
  our_SDL_fill_rect_wrapper ( Screen , & Bar_Rect , FillColor ) ;

  Bar_Rect . x = 200 + 2 * Percentage ;
  Bar_Rect . y = 200 ;
  Bar_Rect . w = 200 - 2 * Percentage ;
  Bar_Rect . h = 30 ;
  our_SDL_fill_rect_wrapper ( Screen , & Bar_Rect , 0 ) ;

  SDL_SetClipRect( Screen , NULL );

  PrintString ( Screen , 200 + 80 , 200 + 4 , "%d%%", Percentage ) ;

  our_SDL_update_rect_wrapper ( Screen , 200 , 200 , 200 , 30  ) ;

}; // void ShowStartupPercentage ( int Percentage )

/* ----------------------------------------------------------------------
 * This function can be used to play a generic title file, containing 
 * 
 *  1. a background picture name
 *  2. a background music to play
 *  3. some text to display in a scrolling fashion
 *
 * ---------------------------------------------------------------------- */
void
PlayATitleFile ( char* Filename )
{
  char* fpath;
  char* TitleFilePointer;
  // int ScrollEndLine = User_Rect.y;	// endpoint for scrolling...
  int ScrollEndLine = 480;	// endpoint for scrolling...
  char* NextSubsectionStartPointer;
  char* PreparedBriefingText;
  char* TerminationPointer;
  char* TitlePictureName;
  char* TitleSongName;
  int ThisTextLength;

  while ( SpacePressed() );

  //--------------------
  // Now its time to start loading the title file...
  //
  fpath = find_file ( Filename , MAP_DIR , FALSE );
  TitleFilePointer = 
    ReadAndMallocAndTerminateFile( fpath , "*** END OF TITLE FILE *** LEAVE THIS TERMINATOR IN HERE ***" ) ;

  TitleSongName = ReadAndMallocStringFromData ( TitleFilePointer, "The title song in the sound subdirectory for this mission is : " , "\n" ) ;

  SwitchBackgroundMusicTo ( TitleSongName );

  TitlePictureName = ReadAndMallocStringFromData ( TitleFilePointer, "The title picture in the graphics subdirectory for this mission is : " , "\n" ) ;

  SDL_SetClipRect ( Screen, NULL );
  Me[0].status=BRIEFING;
  SetCurrentFont( Para_BFont );

  NextSubsectionStartPointer = TitleFilePointer;
  while ( ( NextSubsectionStartPointer = strstr ( NextSubsectionStartPointer, "*** START OF PURE SCROLLTEXT DATA ***")) 
	  != NULL )
    {
      NextSubsectionStartPointer += strlen ( "*** START OF PURE SCROLLTEXT DATA ***" );
      if ( (TerminationPointer=strstr ( NextSubsectionStartPointer, "*** END OF PURE SCROLLTEXT DATA ***")) == NULL)
	{
	  DebugPrintf (1, "\n\nvoid PlayATitleFile(...): Unterminated Subsection in Mission briefing....Terminating...");
	  Terminate(ERR);
	}
      ThisTextLength=TerminationPointer-NextSubsectionStartPointer;
      PreparedBriefingText = MyMalloc (ThisTextLength + 10);
      strncpy ( PreparedBriefingText , NextSubsectionStartPointer , ThisTextLength );
      PreparedBriefingText[ThisTextLength]=0;
      fflush(stdout);
      // ScrollText ( PreparedBriefingText, SCROLLSTARTX, SCROLLSTARTY, ScrollEndLine , TitlePictureName );
      ScrollText ( PreparedBriefingText, SCROLLSTARTX, SCROLLSTARTY, ScrollEndLine , NE_TITLE_PIC_BACKGROUND_CODE );
      free ( PreparedBriefingText );
    }

  ClearGraphMem ();
  DisplayBanner (NULL, NULL,  BANNER_FORCE_UPDATE ); 
  our_SDL_flip_wrapper( Screen );
  
}; // void PlayATitleFile ( char* Filename )

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

#define BULLET_SECTION_BEGIN_STRING "*** Start of Bullet Data Section: ***" 
#define BULLET_SECTION_END_STRING "*** End of Bullet Data Section: ***" 
#define NEW_BULLET_TYPE_BEGIN_STRING "** Start of new bullet specification subsection **"

#define BULLET_RECHARGE_TIME_BEGIN_STRING "Time is takes to recharge this bullet/weapon in seconds :"
#define BULLET_SPEED_BEGIN_STRING "Flying speed of this bullet type :"
#define BULLET_DAMAGE_BEGIN_STRING "Damage cause by a hit of this bullet type :"
#define BULLET_ONE_SHOT_ONLY_AT_A_TIME "Cannot fire until previous bullet has been deleted : "
#define BULLET_BLAST_TYPE_CAUSED_BEGIN_STRING "Type of blast this bullet causes when crashing e.g. against a wall :"

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

  DebugPrintf (1, "\nEnd of Get_Bullet_Data ( char* DataPointer ) reached.");
} // void Get_Bullet_Data ( char* DataPointer );

/* ----------------------------------------------------------------------
 * Delete all events and event triggers
 * ---------------------------------------------------------------------- */
void
clear_out_all_events_and_actions( void )
{
  int i;

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

      // Maybe the triggered action will change some obstacle on some level...
      AllTriggeredActions[i].modify_obstacle_with_label="";
      AllTriggeredActions[i].modify_obstacle_to_type=-1;

      // Maybe the triggered event consists of the map beeing changed at some tile
      AllTriggeredActions[i].ChangeMapLevel=-1;
      AllTriggeredActions[i].ChangeMapLocation.x=-1;
      AllTriggeredActions[i].ChangeMapLocation.y=-1;
      AllTriggeredActions[i].ChangeMapTo=-1;
      AllTriggeredActions[i].AssignWhichMission=-1;
      // Maybe the triggered event consists of ??????
    }
}; // void clear_out_all_events_and_actions( void )

#define EVENT_TRIGGER_BEGIN_STRING "* Start of an Event Trigger Subsection *"
#define EVENT_TRIGGER_END_STRING "* End of this Event Trigger Subsection *"
#define EVENT_ACTION_BEGIN_STRING "* Start of an Event Action Subsection *"
#define EVENT_ACTION_END_STRING "* End of this Event Action Subsection *"

#define EVENT_ACTION_MAPCHANGE_POS_X_STRING "Mapchange at X="
#define EVENT_ACTION_MAPCHANGE_POS_Y_STRING " Y="
#define EVENT_ACTION_MAPCHANGE_MAPLEVEL_STRING " Lev="
#define EVENT_ACTION_MAPCHANGE_TO_WHAT_STRING " to new value="
#define MAPCHANGE_LABEL_STRING "Use map label for map change location=\""

#define EVENT_ACTION_TELEPORT_POS_X_STRING "Teleport to TelX="
#define EVENT_ACTION_TELEPORT_POS_Y_STRING " TelY="
#define EVENT_ACTION_TELEPORT_LEVEL_STRING " TelLev="
#define EVENT_ACTION_TELEPORT_TARGET_LABEL_STRING "Use map label for teleport target=\""

#define EVENT_ACTION_INFLUENCER_SAY_TEXT "Action is Influencer say=\""
#define EVENT_ACTION_ASSIGN_WHICH_MISSION "Action is mission assignment="
#define ACTION_LABEL_INDICATION_STRING "Action label for this action=\""

#define EVENT_TRIGGER_POS_X_STRING "Influencer must be at X="
#define EVENT_TRIGGER_POS_Y_STRING " Y="
#define EVENT_TRIGGER_POS_MAPLEVEL_STRING " Lev="
#define EVENT_TRIGGER_DELETED_AFTER_TRIGGERING "Delete the event trigger after it has been triggered="
#define TRIGGER_WHICH_TARGET_LABEL "Event Action to be triggered by this trigger=\""
#define EVENT_TRIGGER_LABEL_STRING "Use map location from map label=\""

#define MODIFY_OBSTACLE_WITH_LABEL_STRING "modify_obstacle_with_label=\""
#define MODIFY_OBSTACLE_TO_TYPE_STRING "modify_obstacle_to_type="

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
decode_all_event_actions ( char* EventSectionPointer )
{
  char *EventPointer;
  char *EndOfEvent;
  int EventActionNumber;
  char* TempMapLabelName;
  location TempLocation;
  char temp_save_char;

  EventPointer=EventSectionPointer;
  EventActionNumber=0;
  while ( ( EventPointer = strstr ( EventPointer , EVENT_ACTION_BEGIN_STRING ) ) != NULL)
    {
      DebugPrintf(1, "\nBegin of a new Event Action Section found. Good. ");
      EventPointer += strlen( EVENT_ACTION_BEGIN_STRING ) + 1;

      // EndOfEvent = LocateStringInData ( EventSectionPointer , EVENT_ACTION_END_STRING );
      EndOfEvent = LocateStringInData ( EventPointer , EVENT_ACTION_END_STRING );

      DebugPrintf (1, "\n\nStarting to read details of this event action section\n\n");

      //--------------------
      // Now we decode the details of this event action section
      //
      AllTriggeredActions[ EventActionNumber].ActionLabel =
	ReadAndMallocStringFromData ( EventPointer , ACTION_LABEL_INDICATION_STRING , "\"" ) ;

      //--------------------
      // Now we read in the map changing position in x and y and level coordinates
      //
      ReadValueFromString( EventPointer , EVENT_ACTION_MAPCHANGE_POS_X_STRING , "%d" , 
			   &AllTriggeredActions[ EventActionNumber ].ChangeMapLocation.x , EndOfEvent );
      ReadValueFromString( EventPointer , EVENT_ACTION_MAPCHANGE_POS_Y_STRING , "%d" , 
			   &AllTriggeredActions[ EventActionNumber ].ChangeMapLocation.y , EndOfEvent );
      ReadValueFromString( EventPointer , EVENT_ACTION_MAPCHANGE_MAPLEVEL_STRING , "%d" , 
			   &AllTriggeredActions[ EventActionNumber ].ChangeMapLevel , EndOfEvent );
      // but maybe there was a label given.  This will override the pure coordinates...
      TempMapLabelName = 
	ReadAndMallocStringFromData ( EventPointer , MAPCHANGE_LABEL_STRING , "\"" ) ;
      if ( strcmp ( TempMapLabelName , "NO_LABEL_DEFINED_YET" ) )
	{
	  DebugPrintf ( 1 , "\nMapchange coordinates overridden by map label %s." , TempMapLabelName );
	  ResolveMapLabelOnShip ( TempMapLabelName , &TempLocation );
	  AllTriggeredActions[ EventActionNumber ] . ChangeMapLocation . x = TempLocation . x ;
	  AllTriggeredActions[ EventActionNumber ] . ChangeMapLocation . y = TempLocation . y ;
	  AllTriggeredActions[ EventActionNumber ] . ChangeMapLevel = TempLocation . level ;
	}
      else
	{
	  DebugPrintf ( 1 , "\nMapchange label unused..." );
	}


      //--------------------
      // Now we see if maybe there was an obstacle label given, that should be used
      // to change an obstacle later.  We take a look if that is the case at all, and
      // if it is, we'll read in the corresponding obstacle label of course.
      //
      temp_save_char = *EndOfEvent ;
      *EndOfEvent = 0 ;
      if ( CountStringOccurences ( EventPointer , MODIFY_OBSTACLE_WITH_LABEL_STRING ) )
	{
	  DebugPrintf ( 1 , "\nOBSTACLE LABEL FOUND IN THIS EVENT ACTION!" );
	  TempMapLabelName = 
	    ReadAndMallocStringFromData ( EventPointer , MODIFY_OBSTACLE_WITH_LABEL_STRING , "\"" ) ;
	  if ( strcmp ( TempMapLabelName , "NO_LABEL_DEFINED_YET" ) )
	    {
	      AllTriggeredActions [ EventActionNumber ] . modify_obstacle_with_label = TempMapLabelName ;
	      DebugPrintf ( 1 , "\nThe label reads: %s." , AllTriggeredActions [ EventActionNumber ] . modify_obstacle_with_label );
	    }
	  else
	    {
	      DebugPrintf ( 0 , "\nERROR:  Improper label given.\n\
Leave out the label entry for obstacles if you don't want to use it!" );
	      Terminate ( ERR );
	    }
	  //--------------------
	  // But if such an obstacle label has been given, we also need to decode the new type that
	  // this obstacle should be made into.  So we do it here:
	  //
	  ReadValueFromString( EventPointer , MODIFY_OBSTACLE_TO_TYPE_STRING , "%d" , 
			       & ( AllTriggeredActions[ EventActionNumber ] . modify_obstacle_to_type ) , EndOfEvent );
	  DebugPrintf ( 1 , "\nObstacle will be modified to type: %d." , AllTriggeredActions[ EventActionNumber ] . modify_obstacle_to_type );
	}
      else
	{
	  DebugPrintf ( 1 , "\nNO OBSTACLE LABEL FOUND IN THIS EVENT ACTION!" );
	}
      *EndOfEvent = temp_save_char;

      //--------------------
      // Now we read in the teleport target position in x and y and level coordinates
      //
      ReadValueFromString( EventPointer , EVENT_ACTION_TELEPORT_POS_X_STRING , "%d" , 
			   &AllTriggeredActions[ EventActionNumber ].TeleportTarget.x , EndOfEvent );
      ReadValueFromString( EventPointer , EVENT_ACTION_TELEPORT_POS_Y_STRING , "%d" , 
			   &AllTriggeredActions[ EventActionNumber ].TeleportTarget.y , EndOfEvent );
      ReadValueFromString( EventPointer , EVENT_ACTION_TELEPORT_LEVEL_STRING , "%d" , 
			   &AllTriggeredActions[ EventActionNumber ].TeleportTargetLevel , EndOfEvent );
      // but maybe there was a label given.  This will override the pure coordinates...
      TempMapLabelName = 
	ReadAndMallocStringFromData ( EventPointer , EVENT_ACTION_TELEPORT_TARGET_LABEL_STRING , "\"" ) ;
      if ( strcmp ( TempMapLabelName , "NO_LABEL_DEFINED_YET" ) )
	{
	  DebugPrintf ( 1 , "\nTeleport target coordinates overridden by map label %s." , TempMapLabelName );
	  ResolveMapLabelOnShip ( TempMapLabelName , &TempLocation );
	  AllTriggeredActions [ EventActionNumber ] . TeleportTarget . x = TempLocation . x ;
	  AllTriggeredActions [ EventActionNumber ] . TeleportTarget . y = TempLocation . y ;
	  AllTriggeredActions [ EventActionNumber ] . TeleportTargetLevel = TempLocation . level ;
	}
      else
	{
	  DebugPrintf ( 1 , "\nTeleport target label unused..." );
	}

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

}; // void 

/* ---------------------------------------------------------------------- 
 *
 *
 * ---------------------------------------------------------------------- */
void
decode_all_event_triggers ( char* EventSectionPointer )
{
  char *EventPointer;
  char *EndOfEvent;
  int EventTriggerNumber;
  char* TempMapLabelName;
  location TempLocation;

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

      // Now we read in the triggering position in x and y and z coordinates
      ReadValueFromString( EventPointer , EVENT_TRIGGER_POS_X_STRING , "%d" , 
			   &AllEventTriggers[ EventTriggerNumber ].Influ_Must_Be_At_Point.x , EndOfEvent );
      ReadValueFromString( EventPointer , EVENT_TRIGGER_POS_Y_STRING , "%d" , 
			   &AllEventTriggers[ EventTriggerNumber ].Influ_Must_Be_At_Point.y , EndOfEvent );
      ReadValueFromString( EventPointer , EVENT_TRIGGER_POS_MAPLEVEL_STRING , "%d" , 
			   &AllEventTriggers[ EventTriggerNumber ].Influ_Must_Be_At_Level , EndOfEvent );
      // but maybe there was a label given.  This will override the pure coordinates...
      TempMapLabelName = 
	ReadAndMallocStringFromData ( EventPointer , EVENT_TRIGGER_LABEL_STRING , "\"" ) ;
      if ( strcmp ( TempMapLabelName , "NO_LABEL_DEFINED_YET" ) )
	{
	  DebugPrintf ( 1 , "\nTrigger coordinates overridden by map label %s." , TempMapLabelName );
	  ResolveMapLabelOnShip ( TempMapLabelName , &TempLocation );
	  AllEventTriggers [ EventTriggerNumber ] . Influ_Must_Be_At_Point . x = TempLocation . x ;
	  AllEventTriggers [ EventTriggerNumber ] . Influ_Must_Be_At_Point . y = TempLocation . y ;
	  AllEventTriggers[ EventTriggerNumber ] . Influ_Must_Be_At_Level = TempLocation . level ;
	}
      else
	{
	  DebugPrintf ( 1 , "\nTrigger label unused..." );
	}


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

}; // void

/* ----------------------------------------------------------------------
 * This function reads in the game events, i.e. the locations and conditions
 * under which some actions are triggered.
 * ---------------------------------------------------------------------- */
void 
GetEventsAndEventTriggers ( char* EventsAndEventTriggersFilename )
{
  char* EventSectionPointer;
  char* fpath;

  //--------------------
  // At first we clear out any garbage that might randomly reside in the current event
  // and action structures...
  //
  clear_out_all_events_and_actions();

  //--------------------
  // Now its time to start loading the event file...
  //
  fpath = find_file ( EventsAndEventTriggersFilename , MAP_DIR , FALSE );
  EventSectionPointer = 
    ReadAndMallocAndTerminateFile( fpath , 
				   "*** END OF EVENT ACTION AND EVENT TRIGGER FILE *** LEAVE THIS TERMINATOR IN HERE ***" 
				   ) ;

  //--------------------
  // At first we decode ALL THE EVENT ACTIONS not the TRIGGERS!!!!
  //
  decode_all_event_actions ( EventSectionPointer );

  //--------------------
  // Now we decode ALL THE EVENT TRIGGERS not the ACTIONS!!!!
  //
  decode_all_event_triggers ( EventSectionPointer );

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
  double experience_reward_calibrator;
  double range_of_vision_calibrator;

#define MAXSPEED_CALIBRATOR_STRING "Common factor for all droids maxspeed values: "
#define ACCELERATION_CALIBRATOR_STRING "Common factor for all droids acceleration values: "
#define MAXENERGY_CALIBRATOR_STRING "Common factor for all droids maximum energy values: "
#define ENERGYLOSS_CALIBRATOR_STRING "Common factor for all droids energyloss values: "
#define AGGRESSION_CALIBRATOR_STRING "Common factor for all droids aggression values: "
#define EXPERIENCE_REWARD_CALIBRATOR_STRING "Common factor for all droids experience_reward values: "
#define RANGE_OF_VISION_CALIBRATOR_STRING "Common factor for all droids range of vision: "

#define ROBOT_SECTION_BEGIN_STRING "*** Start of Robot Data Section: ***" 
#define ROBOT_SECTION_END_STRING "*** End of Robot Data Section: ***" 
#define NEW_ROBOT_BEGIN_STRING "** Start of new Robot: **" 
#define DROIDNAME_BEGIN_STRING "Droidname: "
#define PORTRAIT_FILENAME_WITHOUT_EXT "Droid portrait file name (without extension) to use=\""

#define DROID_PORTRAIT_ROTATION_SERIES_NAME_PREFIX "Droid uses portrait rotation series with prefix=\""

#define MAXSPEED_BEGIN_STRING "Maximum speed of this droid: "
#define CLASS_BEGIN_STRING "Class of this droid: "
#define ACCELERATION_BEGIN_STRING "Maximum acceleration of this droid: "
#define MAXENERGY_BEGIN_STRING "Maximum energy of this droid: "
#define MAXMANA_BEGIN_STRING "Maximum mana of this droid: "
#define LOSEHEALTH_BEGIN_STRING "Rate of energyloss under influence control: "
#define GUN_BEGIN_STRING "Weapon type this droid uses: "
#define AGGRESSION_BEGIN_STRING "Aggression rate of this droid: "
#define BASE_PHYSICAL_DAMAGE_BEGIN_STRING "Physical (base) damage an attack of this droid will do: "
#define FLASHIMMUNE_BEGIN_STRING "Is this droid immune to disruptor blasts? "
#define EXPERIENCE_REWARD_BEGIN_STRING "Experience_Reward gained for destroying one of this type: "
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
#define DROID_DEATH_SOUND_FILE_NAME "Death sound file name=\""
#define DROID_ATTACK_ANIMATION_SOUND_FILE_NAME "Attack animation sound file name=\""
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

  // Now we read in the experience_reward calibration factor for all droids
  ReadValueFromString( RobotPointer , EXPERIENCE_REWARD_CALIBRATOR_STRING , "%lf" , 
		       &experience_reward_calibrator , EndOfDataPointer );

  // Now we read in the range of vision calibration factor for all droids
  ReadValueFromString( RobotPointer , RANGE_OF_VISION_CALIBRATOR_STRING , "%lf" , 
		       &range_of_vision_calibrator , EndOfDataPointer );

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

      //--------------------
      // Now we read in the file name of the portrait file for this droid.  
      // Is should be enclosed in double-quotes.
      //
      Druidmap[RobotIndex].portrait_filename_without_ext =
	ReadAndMallocStringFromData ( RobotPointer , PORTRAIT_FILENAME_WITHOUT_EXT , "\"" ) ;

      //--------------------
      // Now we read in the prefix of the file names in the rotation series
      // to use for the console droid rotation
      Druidmap [ RobotIndex ] . droid_portrait_rotation_series_prefix =
	ReadAndMallocStringFromData ( RobotPointer , DROID_PORTRAIT_ROTATION_SERIES_NAME_PREFIX , "\"" ) ;

      //--------------------
      // Now we read in the file name of the death sound for this droid.  
      // Is should be enclosed in double-quotes.
      //
      Druidmap [ RobotIndex ] . droid_death_sound_file_name =
	ReadAndMallocStringFromData ( RobotPointer , DROID_DEATH_SOUND_FILE_NAME , "\"" ) ;

      //--------------------
      // Now we read in the file name of the attack animation sound for this droid.  
      // Is should be enclosed in double-quotes.
      //
      Druidmap [ RobotIndex ] . droid_attack_animation_sound_file_name =
	ReadAndMallocStringFromData ( RobotPointer , DROID_ATTACK_ANIMATION_SOUND_FILE_NAME , "\"" ) ;

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

      // Now we read in the aggression rate of this droid.
      ReadValueFromString( RobotPointer , BASE_PHYSICAL_DAMAGE_BEGIN_STRING , "%f" , 
			   & Druidmap [ RobotIndex ] . physical_damage , EndOfDataPointer );

      // Now we read in range of vision of this droid
      ReadValueFromString( RobotPointer , "Range of vision of this droid=" , "%f" , 
			   &Druidmap[RobotIndex].range_of_vision , EndOfDataPointer );

      // Now we read in range of vision of this droid
      ReadValueFromString( RobotPointer , "Time spent eyeing Tux=" , "%f" , 
			   &Druidmap[RobotIndex].time_spent_eyeing_tux , EndOfDataPointer );

      // Now we read in range of vision of this droid
      ReadValueFromString( RobotPointer , "Minimal distance hostile bots are tolerated=" , "%f" , 
			   &Druidmap[RobotIndex].minimal_range_hostile_bots_are_ignored , EndOfDataPointer );

      // Now we read in the flash immunity of this droid.
      ReadValueFromString( RobotPointer , FLASHIMMUNE_BEGIN_STRING , "%d" , 
			   &Druidmap[RobotIndex].flashimmune , EndOfDataPointer );

      // Now we experience_reward to be had for destroying one droid of this type
      ReadValueFromString( RobotPointer , EXPERIENCE_REWARD_BEGIN_STRING , "%d" , 
			   &Druidmap[RobotIndex].experience_reward, EndOfDataPointer );

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

      // Now we read in the number of plasma transistors
      ReadValueFromString( RobotPointer , "Number of Plasma Transistors=" , "%d" , 
			   &Druidmap[RobotIndex].amount_of_plasma_transistors , EndOfDataPointer );

      // Now we read in the number of plasma transistors
      ReadValueFromString( RobotPointer , "Number of Superconductors=" , "%d" , 
			   &Druidmap[RobotIndex].amount_of_superconductors , EndOfDataPointer );

      // Now we read in the number of plasma transistors
      ReadValueFromString( RobotPointer , "Number of Antimatter-Matter Converters=" , "%d" , 
			   &Druidmap[RobotIndex].amount_of_antimatter_converters , EndOfDataPointer );

      // Now we read in the number of plasma transistors
      ReadValueFromString( RobotPointer , "Number of Entropy Inverters=" , "%d" , 
			   &Druidmap[RobotIndex].amount_of_entropy_inverters , EndOfDataPointer );

      // Now we read in the number of plasma transistors
      ReadValueFromString( RobotPointer , "Number of Tach. Condensators=" , "%d" , 
			   &Druidmap[RobotIndex].amount_of_tachyon_condensators , EndOfDataPointer );

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

      // Now we read in the modifier, that increases/decreases the chance of this robot getting hit
      ReadValueFromString( RobotPointer , "Time to recover after getting hit=" , "%lf" , 
			   &Druidmap[RobotIndex] . recover_time_after_getting_hit , EndOfDataPointer );

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

  for ( i = 0 ; i < Number_Of_Droid_Types ; i++ ) 
    {
      Druidmap [ i ] . maxspeed *= maxspeed_calibrator;
      Druidmap [ i ] . maxenergy *= maxenergy_calibrator;
      Druidmap [ i ] . lose_health *= energyloss_calibrator;
      Druidmap [ i ] . aggression *= aggression_calibrator;
      Druidmap [ i ] . experience_reward *= experience_reward_calibrator;
      Druidmap [ i ] . range_of_vision *= range_of_vision_calibrator;

      Druidmap [ i ] . weapon_item . currently_held_in_hand = FALSE ;
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

#define INIT_GAME_DATA_DEBUG 1 

  DebugPrintf ( 2 , "\nint Init_Game_Data ( char* Datafilename ) called." );

  //--------------------
  // First we load the general game constants
  //
  fpath = find_file ( "freedroid.ruleset" , MAP_DIR, FALSE);
  DebugPrintf ( INIT_GAME_DATA_DEBUG , "\nvoid Init_Game_Data:  Data will be taken from file : %s.  Commencing... \n" ,
		fpath );
  Data = ReadAndMallocAndTerminateFile( fpath , "*** End of this Freedroid data File ***" ); 
  Get_General_Game_Constants( Data );
  free ( Data );

  //--------------------
  // Item archetypes must be loaded too
  //
  fpath = find_file ( "freedroid.item_archetypes" , MAP_DIR , FALSE );
  DebugPrintf ( INIT_GAME_DATA_DEBUG , "\nvoid Init_Game_Data:  Data will be taken from file : %s. Commencing... \n" ,
		fpath );
  Data = ReadAndMallocAndTerminateFile( fpath , "*** End of this Freedroid data File ***" ) ;
  Get_Item_Data ( Data );
  free ( Data );

  //--------------------
  // Time to eat some droid archetypes...
  //
  fpath = find_file ( "freedroid.droid_archetypes" , MAP_DIR , FALSE );
  DebugPrintf ( INIT_GAME_DATA_DEBUG , "\nvoid Init_Game_Data:  Data will be taken from file : %s. Commencing... \n" ,
		fpath );
  Data = ReadAndMallocAndTerminateFile( fpath , "*** End of this Freedroid data File ***" ) ;
  Get_Robot_Data ( Data );
  free ( Data );

  //--------------------
  // Now finally it's time for all the bullet data...
  //
  fpath = find_file ( "freedroid.bullet_archetypes" , MAP_DIR, FALSE);
  DebugPrintf ( INIT_GAME_DATA_DEBUG , "\nvoid Init_Game_Data:  Data will be taken from file : %s. Commencing... \n" ,
		fpath );
  Data = ReadAndMallocAndTerminateFile( fpath , "*** End of this Freedroid data File ***" ) ;
  Get_Bullet_Data ( Data );
  free ( Data );

}; // int Init_Game_Data ( void )

char copyright[] = "\nCopyright (C) 2003 Johannes Prix, Reinhard Prix\n\
Freedroid comes with NO WARRANTY to the extent permitted by law.\n\
You may redistribute copies of Freedroid\n\
under the terms of the GNU General Public License.\n\
For more information about these matters, see the file named COPYING.\n";


char usage_string[] ="\
Usage: freedroid [-v|--version] \n\
                 [-s|--sound] [-q|--nosound] \n\
                 [-o|--open_gl] [-n|--no_open_gl]\n\
                 [-f|--fullscreen] [-w|--window]\n\
                 [-j|--sensitivity]\n\
                 [-d|--debug=LEVEL]\n\
\n\
Please report bugs either by entering them into the bug-tracking\n\
system on our sourceforge-website via this link:\n\n\
http://sourceforge.net/projects/freedroid/\n\n\
or EVEN BETTER, report them by sending e-mail to:\n\n\
freedroid-discussion@lists.sourceforge.net\n\n\
Thanks a lot in advance, the Freedroid dev team.\n\n";

/* -----------------------------------------------------------------
 *  parse command line arguments and set global switches 
 *  exit on error, so we don't need to return success status
 * -----------------------------------------------------------------*/
void
ParseCommandLine (int argc, char *const argv[])
{
  int c;

  static struct option long_options[] = {
    {"version",     0, 0,  'v'},
    {"help", 	    0, 0,  'h'},
    {"open_gl",     0, 0,  'o'},
    {"no_open_gl",  0, 0,  'n'},
    {"nosound",     0, 0,  'q'},
    {"sound", 	    0, 0,  's'},
    {"debug", 	    2, 0,  'd'},
    {"window",      0, 0,  'w'},
    {"fullscreen",  0, 0,  'f'},
    {"sensitivity", 1, 0,  'j'},
    { 0, 	    0, 0,    0}
  };

  while (1)
    {
      c = getopt_long (argc, argv, "vonqst:h?d::wfj:", long_options, NULL);
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
	  printf ( usage_string );
	  exit ( 0 );
	  break;

	case 'o':
	  use_open_gl = TRUE;
	  break;

	case 'n':
	  use_open_gl = FALSE;
	  break;

	case 'q':
	  sound_on = FALSE;
	  break;

	case 's':
	  sound_on = TRUE;
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
}				/* ParseCommandLine */


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
GetQuestList ( char* QuestListFilename )
{
  char *EndOfMissionTargetPointer;
  char *NextEventPointer;
  int MissionTargetIndex = 0;
  int NumberOfEventsToTriggerAtThisAssignment;
  int NumberOfEventsToTriggerAtThisCompletition;
  int ActionNr;
  char* ActionLabel;
  char* MissionTargetPointer;
  char* fpath;
  char InnerPreservedLetter=0;

#define MISSION_TARGET_SUBSECTION_START_STRING "** Start of this mission target subsection **"
#define MISSION_TARGET_SUBSECTION_END_STRING "** End of this mission target subsection **"

#define MISSION_TARGET_NAME_INITIALIZER "Mission Name=\""

#define MISSION_AUTOMATICALLY_ASSIGN_STRING "Assign this mission to influencer automatically at start : "
#define MISSION_TARGET_FETCH_ITEM_STRING "Mission target is to fetch item : "
#define MISSION_TARGET_KILL_ALL_STRING "Mission target is to kill all droids : "
#define MISSION_TARGET_KILL_CLASS_STRING "Mission target is to kill class of droids : "
#define MISSION_TARGET_KILL_ONE_STRING "Mission target is to kill droids with marker : "
#define MISSION_TARGET_MUST_CLEAR_FIRST_LEVEL "Mission target is to kill all hostile droids this first level : "
#define MISSION_TARGET_MUST_CLEAR_SECOND_LEVEL "Mission target is to also kill all hostile droids on second level : "
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
  // At first we must load the quest list file given...
  //
  fpath = find_file ( QuestListFilename , MAP_DIR , FALSE );
  MissionTargetPointer = 
    ReadAndMallocAndTerminateFile( fpath , "*** END OF QUEST LIST *** LEAVE THIS TERMINATOR IN HERE ***" ) ;

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

      //--------------------
      // We need to add an inner terminator here, so that the strstr operation
      // below will know where to stop within this subsection.
      //
      InnerPreservedLetter = * EndOfMissionTargetPointer;
      * EndOfMissionTargetPointer = 0 ;

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

      ReadValueFromString( MissionTargetPointer , MISSION_TARGET_MUST_CLEAR_FIRST_LEVEL , "%d" , 
			   &Me[0].AllMissions[ MissionTargetIndex ]. must_clear_first_level , EndOfMissionTargetPointer );

      ReadValueFromString( MissionTargetPointer , MISSION_TARGET_MUST_CLEAR_SECOND_LEVEL , "%d" , 
			   &Me[0].AllMissions[ MissionTargetIndex ]. must_clear_second_level , EndOfMissionTargetPointer );
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

	  Me [ 0 ] . AllMissions [ MissionTargetIndex ] . ListOfActionsToBeTriggeredAtCompletition [ NumberOfEventsToTriggerAtThisCompletition ] = GiveNumberToThisActionLabel ( ActionLabel );
	  
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

      //--------------------
      // We restore the termination character we added before, even if that
      // is maybe not really nescessary...
      //
      * EndOfMissionTargetPointer = InnerPreservedLetter ;


    } // while mission target found...


  //--------------------
  // Finally we record the number of mission targets scanned and are done with this function
  DebugPrintf ( 1 , "\nNUMBER OF MISSION TARGETS FOUND: %d.\n" , MissionTargetIndex );
  fflush( stdout );

}; // void Get_Mission_Targets( ... )

/* ----------------------------------------------------------------------
 * Now we initialize the skills of the new hero...
 * ---------------------------------------------------------------------- */
void
InitInfluencerStartupSkills( int PlayerNum )
{
  int i ;

  Me[ PlayerNum ].readied_skill = 0;
  for ( i = 0 ; i < NUMBER_OF_SKILLS ; i ++ ) 
    {
      Me[ PlayerNum ].SkillLevel [ i ] = 0 ;
      Me[ PlayerNum ].base_skill_level [ i ] = 0 ;
    }
  Me[ PlayerNum ] . SkillLevel [ 0 ] = 1 ;
  Me[ PlayerNum ].base_skill_level [  0 ] = 1 ; // transfer mode present...
  Me[ PlayerNum ].base_skill_level [  1 ] = 1 ; // loot chest present...
  Me[ PlayerNum ].base_skill_level [  2 ] = 1 ; // repair skill present...
  Me[ PlayerNum ].base_skill_level [  3 ] = 1 ; // weapon skill present...
  Me[ PlayerNum ].base_skill_level [  4 ] = 0 ; // force-to-energy (HEALING) NOT present...
  Me[ PlayerNum ].base_skill_level [  5 ] = 0 ; // teleport home disabled for consistency...
  Me[ PlayerNum ].base_skill_level [  6 ] = 0 ; // firy-bolt NOT present...
  /*
  Me[ PlayerNum ].base_skill_level [  7 ] = 1 ; 
  Me[ PlayerNum ].base_skill_level [  8 ] = 1 ;
  Me[ PlayerNum ].base_skill_level [  9 ] = 1 ;
  Me[ PlayerNum ].base_skill_level [ 10 ] = 1 ;
  Me[ PlayerNum ].base_skill_level [ 11 ] = 1 ;
  Me[ PlayerNum ].base_skill_level [ 12 ] = 1 ;
  Me[ PlayerNum ].base_skill_level [ 13 ] = 1 ;
  Me[ PlayerNum ].base_skill_level [ 14 ] = 1 ;
  Me[ PlayerNum ].base_skill_level [ 14 ] = 1 ;
  */

  GameConfig.spell_level_visible = 0;

  Me [ PlayerNum ] . melee_weapon_skill = 0 ;
  Me [ PlayerNum ] . ranged_weapon_skill = 0 ;
  Me [ PlayerNum ] . spellcasting_skill = 0 ;
  Me [ PlayerNum ] . hacking_skill = 0 ;

}; // void InitInfluencerStartupSkills( int PlayerNum )

/* ----------------------------------------------------------------------
 * Now we disable all chat flags (i.e. the Tux hasn't spoken to
 * that person at all) for all the non-player-characters in the game,
 * except for the 0-chat alternative, which is always set to open.  WHY???????
 * ---------------------------------------------------------------------- */
void
InitInfluencerChatFlags( int PlayerNum )
{
  int i , j;

  for ( i = 0 ; i < MAX_ANSWERS_PER_PERSON ; i ++ ) 
    {
      for ( j = 0 ; j < MAX_PERSONS ; j ++ )
	{
	  Me[ PlayerNum ] . Chat_Flags [ j ] [ i ] = 0 ;
	  Me[ PlayerNum ] . Chat_Flags [ j ] [ 0 ] = 1 ; // we always allow to say something in the beginning...
	}
    }

}; // void InitInfluencerChatFlags( int PlayerNum )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
InitInfluencerPasswordsAndClearances( int PlayerNum )
{
  int j;

  //--------------------
  // At the beginning, the Tux shouldn't have any passwords
  // or security clearances on him, and that's was we ensure here.
  //
  for ( j = 0 ; j < MAX_CLEARANCES ; j ++ )
    {
      Me [ PlayerNum ] . clearance_list [ j ] = 0 ;
    }
  j=0;
  Me [ PlayerNum ] . clearance_list [ j ] = 1 ; j++;
  Me [ PlayerNum ] . clearance_list [ j ] = 2 ; j++;
  Me [ PlayerNum ] . clearance_list [ j ] = 3 ; j++;
  Me [ PlayerNum ] . clearance_list [ j ] = 4 ; j++;
  Me [ PlayerNum ] . clearance_list [ j ] = 5 ; j++;
  Me [ PlayerNum ] . clearance_list [ j ] = 6 ; j++;
  Me [ PlayerNum ] . clearance_list [ j ] = 7 ; j++;
  Me [ PlayerNum ] . clearance_list [ j ] = 8 ; j++;
  Me [ PlayerNum ] . clearance_list [ j ] = 9 ; j++;
  Me [ PlayerNum ] . clearance_list [ j ] = 10 ; j++;
  Me [ PlayerNum ] . clearance_list [ j ] = 11 ; j++;
  Me [ PlayerNum ] . clearance_list [ j ] = 12 ; j++;
  Me [ PlayerNum ] . clearance_list [ j ] = 13 ; j++;
  Me [ PlayerNum ] . clearance_list [ j ] = 14 ; j++;
  Me [ PlayerNum ] . clearance_list [ j ] = 15 ; j++;
  Me [ PlayerNum ] . clearance_list [ j ] = 16 ; j++;
  Me [ PlayerNum ] . clearance_list [ j ] = 17 ; j++;
  Me [ PlayerNum ] . clearance_list [ j ] = 18 ; j++;
  Me [ PlayerNum ] . clearance_list [ j ] = 19 ; j++;
  Me [ PlayerNum ] . clearance_list [ j ] = 20 ; j++;
  Me [ PlayerNum ] . clearance_list [ j ] = 21 ; j++;
  Me [ PlayerNum ] . clearance_list [ j ] = 22 ; j++;
  Me [ PlayerNum ] . clearance_list [ j ] = 23 ; j++;

  for ( j = 0 ; j < MAX_PASSWORDS ; j ++ )
    {
      strcpy ( Me [ PlayerNum ] . password_list [ j ] , "" ) ;
    }
  strcpy ( Me [ PlayerNum ] . password_list [ 0 ] , "Tux Himself" ) ;
  strcpy ( Me [ PlayerNum ] . password_list [ 1 ] , "Tux Dummy1" ) ;
  strcpy ( Me [ PlayerNum ] . password_list [ 2 ] , "Tux Dummy2" ) ;
  strcpy ( Me [ PlayerNum ] . password_list [ 3 ] , "Tux Energy" ) ;
  strcpy ( Me [ PlayerNum ] . password_list [ 4 ] , "Tux Idenfity" ) ;

}; // void InitInfluencerPasswordsAndClearances( int PlayerNum )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
InitHarmlessTuxStatusVariables( int PlayerNum )
{
  int i;

  Me[ PlayerNum ].type = DRUID001;
  Me[ PlayerNum ].speed.x = 0;
  Me[ PlayerNum ].speed.y = 0;
  Me[ PlayerNum ].status = MOBILE;
  Me[ PlayerNum ].phase = 0;
  Me[ PlayerNum ].MissionTimeElapsed=0;
  Me[ PlayerNum ].Current_Victim_Resistance_Factor=1;
  Me[ PlayerNum ].FramesOnThisLevel=0;
  Me[ PlayerNum ].weapon_swing_time = (-1);  // currently not swinging this means...
  Me[ PlayerNum ].got_hit_time = (-1);  // currently not stunned and needing time to recover...
  Me[ PlayerNum ].points_to_distribute = 0;
  Me[ PlayerNum ].ExpRequired = 1500;
  for ( i = 0 ; i < 1000 ; i ++ ) Me[ PlayerNum ].KillRecord[ i ] = 0;
  for ( i = 0 ; i < MAX_LEVELS ; i ++ ) 
    {
      Me [ PlayerNum ] . HaveBeenToLevel [ i ] = FALSE ;
      Me [ PlayerNum ] . time_since_last_visit_or_respawn [ i ] = (-1) ;
    }
  for ( i = 0 ; i < MAX_ITEMS_PER_LEVEL ; i ++ ) 
    {
      Me[ PlayerNum ].DetectedItemList[i].x = 0 ;
      Me[ PlayerNum ].DetectedItemList[i].y = 0 ;
    }
  Me[ PlayerNum ].Experience = 1;
  Me[ PlayerNum ].exp_level = 1;
  Me[ PlayerNum ].Gold = 0 ; // formerly this was starting funds 100, but no longer... 

}; // void InitHarmlessTuxStatusVariables( int PlayerNum )

/* -----------------------------------------------------------------
 * This function initializes a completely new game within freedroid.
 * In contrast to InitFreedroid, this function should be called 
 * whenever or better before any new game is started.
 * -----------------------------------------------------------------*/
void
PrepareStartOfNewCharacter ( void )
{
  int i , j ;
  int StartingLevel=0;
  int StartingXPos=0;
  int StartingYPos=0;
  int MissionTargetIndex = 0;
  int PlayerNum;
  location StartPosition;
  
  //--------------------
  // At first we do the things that must be done for all
  // missions, regardless of mission file given
  //
  Activate_Conservative_Frame_Computation();
  ThisMessageTime = 0;
  LevelDoorsNotMovedTime = 0.0;
  RespectVisibilityOnMap = TRUE ;

  //--------------------
  // Now the mission file is read into memory.  That means we can start to decode the details given
  // in the body of the mission file.  
  //
  GetEventsAndEventTriggers ( "EventsAndEventTriggers" );

  PlayATitleFile ( "StartOfGame.title" );

  //--------------------
  // We also load the comment for the influencer to say at the beginning of the mission
  //
  Me [ 0 ] . TextToBeDisplayed = "Huh? What?  Where am I?" ;
  Me [ 0 ] . TextVisibleTime = 0;

  //--------------------
  // initialize enemys according to crew file */
  // WARNING!! THIS REQUIRES THE freedroid.ruleset FILE TO BE READ ALREADY, BECAUSE
  // ROBOT SPECIFICATIONS ARE ALREADY REQUIRED HERE!!!!!
  //
  GetCrew ( "ReturnOfTux.droids" ) ;

  // ResolveMapLabelOnShip ( "TuxStartGameSquare" , &StartPosition );
  ResolveMapLabelOnShip ( "NewTuxStartGameSquare" , &StartPosition );
  Me [ 0 ] . pos . x = StartPosition . x ;
  Me [ 0 ] . pos . y = StartPosition . y ;
  Me [ 0 ] . pos . z = StartPosition . level ;

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
  GetQuestList ( "QuestList" );

  // show the banner for the game
  ClearGraphMem();
  DisplayBanner (NULL, NULL,  BANNER_FORCE_UPDATE );
  InitBars = TRUE;

  SwitchBackgroundMusicTo ( curShip.AllLevels [ Me [ 0 ] . pos . z ] ->Background_Song_Name );

  InitHarmlessTuxStatusVariables( 0 );

  InitInfluencerStartupSkills( 0 );

  UpdateAllCharacterStats( 0 );

  InitInfluencerChatFlags( 0 );

  InitInfluencerPasswordsAndClearances( 0 );

  clear_out_intermediate_points ( 0 ) ;

  for ( j = 0 ; j < MAX_COOKIES ; j ++ )
    {
      strcpy ( Me [ 0 ] . cookie_list [ j ] , "" ) ;
    }

  //--------------------
  // Now that the prime character stats have been initialized, we can
  // set these much-varying variables too...
  //
  Me [ 0 ] . energy = Me [ 0 ] . maxenergy;
  Me [ 0 ] . mana = Me [ 0 ] . maxmana;
  Me [ 0 ] . running_power = Me [ 0 ] . max_running_power ;
  Me [ 0 ] . health = Me [ 0 ] . energy;
  Me [ 0 ] . firewait = 0 ;

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
  Item_Held_In_Hand = ( -1 );

  FillInItemProperties ( & ( Me[0].weapon_item ) , TRUE , 0 );
  FillInItemProperties ( & ( Me[0].drive_item ) , TRUE , 0 );


  DebugPrintf ( 0 , "\nPrepareStartOfNewCharacter:  Shuffling droids on all %d levels!" , curShip.num_levels );
  for ( i = 0 ; i < curShip.num_levels ; i ++ )
    {
      // ShuffleEnemys( Me[0].pos.z ); // NOTE: THIS REQUIRES CurLevel TO BE INITIALIZED !! --> NOT ANY MORE!!!
      ShuffleEnemys( i ); // NOTE: THIS REQUIRES CurLevel TO BE INITIALIZED !! --> NOT ANY MORE!!!
    }


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

  Me [ 0 ] . mouse_move_target . x = ( -1 ) ;
  Me [ 0 ] . mouse_move_target . y = ( -1 ) ;
  Me [ 0 ] . mouse_move_target . z = ( -1 ) ;
  Me [ 0 ] . mouse_move_target_is_enemy = ( -1 ) ;
  Me [ 0 ] . mouse_move_target_combo_action_type = NO_COMBO_ACTION_SET ; // what extra action has to be done upon arrival?
  Me [ 0 ] . mouse_move_target_combo_action_parameter = (-1) ; // extra data to use for the combo action

}; // void PrepareStartOfNewCharacter ( char* MissionName )

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
ResetGameConfigToDefaultValues ( void )
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
  strcpy ( GameConfig.Theme_SubPath , "lanzz_theme/" );
  GameConfig.Draw_Framerate=TRUE;
  GameConfig.Draw_Energy=FALSE;
  GameConfig.Draw_Position=FALSE;
  GameConfig.All_Texts_Switch = FALSE;

  GameConfig.terminate_on_missing_speech_sample = FALSE ;
  GameConfig.show_subtitles_in_dialogs = TRUE ;

  GameConfig.enemy_energy_bars_visible = TRUE ;
  
  GameConfig . hog_CPU = TRUE ;
  GameConfig . highlighting_mode_full = TRUE ;
  GameConfig . menu_mode = MENU_MODE_DEFAULT ;
  GameConfig . skip_light_radius = FALSE ; 

  GameConfig . omit_tux_in_level_editor = TRUE ;
  GameConfig . omit_obstacles_in_level_editor = FALSE ;
  GameConfig . omit_enemies_in_level_editor = TRUE ;
  GameConfig . zoom_is_on = FALSE ;

  GameConfig . use_bars_instead_of_energy_o_meter = FALSE ;

  GameConfig . show_blood = TRUE ;
  GameConfig . show_tooltips = TRUE;

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
#ifndef USE_SDL_FRAMERATE
  struct timeval timestamp;
#endif
  int i;

  // feenableexcept ( FE_ALL_EXCEPT );
  // feenableexcept ( FE_DIVBYZERO | FE_INVALID ); // FE_INEXACT | FE_UNDERFLOW | FE_OVERFLOW 
  // fesetexceptflag (const fexcept_t *flagp, int excepts);

  //--------------------
  // We set these dummy values, so that when the title plays (and banner and
  // therefore energy bars are displayed, there won't be any floating point
  // exception problems...)
  //
  Me [ 0 ] . mana = 0 ;
  Me [ 0 ] . maxmana = 10 ;
  Me [ 0 ] . energy = 1 ;
  Me [ 0 ] . maxenergy = 10 ;


  //--------------------
  // It might happen, that the uninitialized AllBullets array contains a 1
  // somewhere and that the bullet is deleted and the surface freed, where
  // it never has been allocated, resulting in a SEGFAULT.  This has never
  // happend, but for security, we add this loop to clean out these important 
  // flags.       It should be sufficient to do this here, since the flag
  // will never be set again if not Surfaces are allocated too and then they
  // can of course also be freed as well.
  //
  level_editor_marked_obstacle = NULL ;
  GameConfig . level_editor_edit_mode = LEVEL_EDITOR_EDIT_FLOOR ;
  Bulletmap=NULL;  // That will cause the memory to be allocated later
  for ( i = 0 ; i < MAXBULLETS ; i++ )
    {
      AllBullets[i].Surfaces_were_generated = FALSE;
      DeleteBullet( i , FALSE );
    }
  for (i = 0; i < MAXBLASTS; i++)
    {
      DeleteBlast( i );
    }
  for (i = 0; i < MAX_ACTIVE_SPELLS; i++)
    {
      DeleteSpell( i );
    }
  ClearEnemys ();
  clear_active_spells();

  ServerMode = FALSE;
  ClientMode = FALSE;
  RespectVisibilityOnMap = TRUE ; 
  timeout_from_item_drop = 0 ; 

  global_ignore_doors_for_collisions_flag = FALSE ;

  Overall_Average=0.041;
  SkipAFewFrames = 0;
  Me[0].TextVisibleTime = 0;
  Me[0].readied_skill = 0;
  Me[0].walk_cycle_phase = 0 ;
  CurLevel = NULL;  // please leave this here.  It indicates, that the map is not yet initialized!!!
  Me[0].TextToBeDisplayed = "Linux Kernel booted.  001 transfer-tech modules loaded.  System up and running.";
  
  // --------------------
  //
  InventorySize.x = INVENTORY_GRID_WIDTH ;
  InventorySize.y = INVENTORY_GRID_HEIGHT ;

  ResetGameConfigToDefaultValues ();

  //Load user config file if it exists...
  LoadGameConfig ();

  Copy_Rect (Full_User_Rect, User_Rect);

  InitTimer ();

  InitVideo ();

  ShowStartupPercentage ( 2 ) ; 

  InitAudio ();

  ShowStartupPercentage ( 4 ) ; 
  
  LoadAllStaticModFiles();

  ShowStartupPercentage ( 8 ) ; 

  //--------------------
  // Now that the music files have been loaded successfully, it's time to set
  // the music and sound volumes accoridingly, i.e. as specifies by the users
  // configuration.
  //
  // THIS MUST NOT BE DONE BEFORE THE SOUND SAMPLES HAVE BEEN LOADED!!
  //
  SetSoundFXVolume( GameConfig.Current_Sound_FX_Volume );

  Init_Joy ();

  ShowStartupPercentage ( 10 ) ; 

  //--------------------
  // Now we prepare the automap data for later use
  //
  GameConfig.Automap_Visible = TRUE;
  ClearAutomapData( );

  ShowStartupPercentage ( 12 ) ; 

  Init_Network ();

  ShowStartupPercentage ( 14 ) ; 

  Init_Game_Data( NULL ); 

  ShowStartupPercentage ( 16 ) ; 

  // The default should be, that no rescaling of the
  // combat window at all is done.
  CurrentCombatScaleFactor = 1;

  /* 
   * Initialise random-number generator in order to make 
   * level-start etc really different at each program start
   */
#ifndef USE_SDL_FRAMERATE
  gettimeofday(&timestamp, NULL);
  srand((unsigned int) timestamp.tv_sec); /* yes, we convert long->int here! */
#endif

  MinMessageTime = 55;
  MaxMessageTime = 850;

  CurLevel = NULL; // please leave this here BEFORE InitPictures

  /* Now fill the pictures correctly to the structs */
  if (!InitPictures ())
    {		
      DebugPrintf (1, "\n Error in InitPictures reported back...\n");
      Terminate(ERR);
    }

  ShowStartupPercentage ( 100 ) ; 

}; // void InitFreedroid ( void ) 

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
  Me [ 0 ] . status = OUT ;
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
  DropItemAt ( Me[0].weapon_item.type , Me[0].pos.x - 0.5 , Me[0].pos.y - 0.5 , -1 , -1 , 0 , 1 );
  DropItemAt ( Me[0].drive_item.type  , Me[0].pos.x + 0.5 , Me[0].pos.y - 0.5 , -1 , -1 , 0 , 1 );
  DropItemAt ( Me[0].shield_item.type , Me[0].pos.x + 0.5 , Me[0].pos.y + 0.5 , -1 , -1 , 0 , 1 );
  DropItemAt ( Me[0].armour_item.type , Me[0].pos.x - 0.5 , Me[0].pos.y + 0.5 , -1 , -1 , 0 , 1 );
  DropItemAt ( Me[0].special_item.type , Me[0].pos.x - 0.5 , Me[0].pos.y , -1 , -1 , 0 , 1 );
  DropItemAt ( Me[0].aux1_item.type , Me[0].pos.x + 0.5 , Me[0].pos.y , -1 , -1 , 0 , 1 );
  DropItemAt ( Me[0].aux2_item.type , Me[0].pos.x , Me[0].pos.y - 0.5 , -1 , -1 , 0 , 1 );
  DropItemAt ( ITEM_MONEY , Me[0].pos.x , Me[0].pos.y , -1 , -1 , 0 , 1 );

  GameOver = TRUE;

  while ( ( SDL_GetTicks() - now < 1000 * WAIT_AFTER_KILLED ) && ( GameOver == TRUE ) )
    {
      StartTakingTimeForFPSCalculation(); 

      AssembleCombatPicture ( DO_SCREEN_UPDATE | SHOW_ITEMS );
      DisplayBanner (NULL, NULL,  0 );
      animate_blasts ();
      MoveBullets ();
      MoveEnemys ();
      for ( j = 0 ; j < MAX_PLAYERS ; j ++ ) MoveLevelDoors ( j );	

      ReactToSpecialKeys();

      for (j = 0; j < MAXBULLETS; j++)
	CheckBulletCollisions (j);

      ComputeFPSForThisFrame();

    }

  DebugPrintf (2, "\nvoid ThouArtDefeated(void): Usual end of function reached.");
  DebugPrintf (1, "\n\n DefeatedDone \n\n");
}; // void ThouArtDefeated(void)

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
#define MIS_COMPLETE_DEBUG 1
  
  //--------------------
  // We do not need to check for mission completed EVERY frame
  // It will be enough to do it now and then..., e.g. every 50th frame
  //
  CheckMissionGrid++;
  if ( ( CheckMissionGrid % 50 ) != 0 ) return;

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

      DebugPrintf ( MIS_COMPLETE_DEBUG , "\nSomething was assigned at all..... MissNum = %d " , MissNum );

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
		   ( AllEnemys[Robot_Counter] . marker == Me[0].AllMissions[ MissNum ].KillOne ) )
		{
		  DebugPrintf ( MIS_COMPLETE_DEBUG , "\nOne of the marked droids is still alive...");
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
      // Continue if the Mission target must_clear_first_level is given but not fullfilled
      //
      if ( Me[0].AllMissions[ MissNum ]. must_clear_first_level != (-1) )
	{
	  // for ( Robot_Counter=0 ; Robot_Counter < MAX_ENEMYS_ON_SHIP ; Robot_Counter++ )
	  for ( Robot_Counter=0 ; Robot_Counter < Number_Of_Droids_On_Ship ; Robot_Counter++ )
	    {
	      if ( ( AllEnemys[Robot_Counter].energy > 0 ) && 
		   ( AllEnemys[Robot_Counter].Status != OUT ) && 
		   ( ! AllEnemys[Robot_Counter]. is_friendly ) && 
		   ( AllEnemys[Robot_Counter] . pos . z == Me[0].AllMissions[ MissNum ].must_clear_first_level ) ) 
		{
		  DebugPrintf ( MIS_COMPLETE_DEBUG , "\nOne bot on that first level is still alive: Nr=%d Lev=%d X=%f Y=%f." , 
				Robot_Counter , AllEnemys[Robot_Counter].pos.z , 
				AllEnemys[Robot_Counter].pos.x , AllEnemys[Robot_Counter].pos.y );
		  goto CheckNextMission;
		}
	    }
	}

      //--------------------
      // Continue if the Mission target must_clear_second_level is given but not fullfilled
      //
      if ( Me[0].AllMissions[ MissNum ]. must_clear_second_level != (-1) )
	{
	  // for ( Robot_Counter=0 ; Robot_Counter < MAX_ENEMYS_ON_SHIP ; Robot_Counter++ )
	  for ( Robot_Counter=0 ; Robot_Counter < Number_Of_Droids_On_Ship ; Robot_Counter++ )
	    {
	      if ( ( AllEnemys[Robot_Counter].energy > 0 ) && 
		   ( AllEnemys[Robot_Counter].Status != OUT ) && 
		   ( ! AllEnemys[Robot_Counter]. is_friendly ) && 
		   ( AllEnemys[Robot_Counter] . pos . z == Me[0].AllMissions[ MissNum ].must_clear_second_level ) ) 
		{
		  DebugPrintf ( MIS_COMPLETE_DEBUG , "\nOne bot on that second level is still alive: Nr=%d Lev=%d X=%f Y=%f." , 
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
	  if ( Me[0].marker != Me[0].AllMissions[ MissNum ].MustBeOne ) 
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



