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

/*----------------------------------------------------------------------
 *
 * Desc: the paraplus initialisation routines
 *
 *----------------------------------------------------------------------*/

#define _init_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"
#include "text.h"
#include "getopt.h"   

void Init_Game_Data( char* Datafilename );
void Get_Bullet_Data ( char* DataPointer );

void FindAllThemes (void);

char* DebriefingText;
char DebriefingSong[500];
char NextMissionName[500];
char Previous_Mission_Name[500];

#define MISSION_COMPLETE_BONUS 1000

/*@Function============================================================
@Desc: This function loads all the constant variables of the game from
       a dat file, that should be optimally human readable.

@Ret: 
* $Function----------------------------------------------------------*/
void
Get_General_Game_Constants (char *data)
{
#define CONSTANTS_SECTION_BEGIN_STRING "*** Start of General Game Constants Section: ***"
#define CONSTANTS_SECTION_END_STRING "*** End of General Game Constants Section: ***"
#define COLLISION_LOSE_ENERGY_CALIBRATOR_STRING "Energy-Loss-factor for Collisions of Influ with hostile robots="
#define BLAST_RADIUS_SPECIFICATION_STRING "Radius of explosions (as far as damage is concerned) in multiples of tiles="
#define DROID_RADIUS_SPECIFICATION_STRING "Droid radius:"
#define BLAST_DAMAGE_SPECIFICATION_STRING "Amount of damage done by contact to a blast per second of time="
#define TIME_FOR_DOOR_MOVEMENT_SPECIFICATION_STRING "Time for the doors to move by one subphase of their movement="

#define DEATHCOUNT_DRAIN_SPEED_STRING "Deathcount drain speed ="
#define ALERT_THRESHOLD_STRING "First alert threshold ="
#define ALERT_BONUS_PER_SEC_STRING "Alert bonus per second ="

  DebugPrintf ( 2 , "\n\nStarting to read contents of General Game Constants section\n\n");

  // read in Alert-related parameters:
  ReadValueFromString (data, DEATHCOUNT_DRAIN_SPEED_STRING, "%f", &DeathCountDrainSpeed);
  ReadValueFromString (data, ALERT_THRESHOLD_STRING, "%d", &AlertThreshold);
  ReadValueFromString (data, ALERT_BONUS_PER_SEC_STRING, "%f", &AlertBonusPerSec);

  // Now we read in the speed calibration factor for all bullets
  ReadValueFromString (data, COLLISION_LOSE_ENERGY_CALIBRATOR_STRING, "%f", 
		       &collision_lose_energy_calibrator);

  // Now we read in the blast radius
  ReadValueFromString( data , BLAST_RADIUS_SPECIFICATION_STRING , "%f" , &Blast_Radius);

  // Now we read in the druid 'radius' in x direction
  ReadValueFromString( data , DROID_RADIUS_SPECIFICATION_STRING , "%f" , &Droid_Radius);

  // Now we read in the blast damage amount per 'second' of contact with the blast
  ReadValueFromString( data ,  BLAST_DAMAGE_SPECIFICATION_STRING , "%f" , &Blast_Damage_Per_Second);

  // Now we read in the time is takes for the door to move one phase 
  ReadValueFromString( data ,  TIME_FOR_DOOR_MOVEMENT_SPECIFICATION_STRING , "%f" , 
		       &Time_For_Each_Phase_Of_Door_Movement);

  DebugPrintf(2 , "\nvoid Get_General_Game_Constants ( void* data ): end of function reached." );

  return;
} // Get_General_Game_Constants ()

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
  int i, size;
  int BulletIndex=0;

  float bullet_speed_calibrator;
  float bullet_damage_calibrator;

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

  // Now that we know how many bullets are defined in freedroid.ruleset, we can allocate
  // a fitting amount of memory, but of course only if the memory hasn't been allocated
  // aready!!!
  //
  // If we would do that in any case, every Init_Game_Data call would destroy the loaded
  // image files AND MOST LIKELY CAUSE A SEGFAULT!!!
  //
  if ( Bulletmap == NULL )
    {
      size = sizeof(bulletspec);
      Bulletmap = MyMalloc ( size * (Number_Of_Bullet_Types + 1) + 1 );
      DebugPrintf (1, "\nWe have counted %d different bullet types in the game data file." , 
		   Number_Of_Bullet_Types );
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
      ReadValueFromString( BulletPointer ,  BULLET_RECHARGE_TIME_BEGIN_STRING , "%f" , 
			   &Bulletmap[BulletIndex].recharging_time);

      // Now we read in the maximal speed this type of bullet can go.
      ReadValueFromString( BulletPointer ,  BULLET_SPEED_BEGIN_STRING , "%f" , 
			   &Bulletmap[BulletIndex].speed);

      // Now we read in the damage this bullet can do
      ReadValueFromString( BulletPointer ,  BULLET_DAMAGE_BEGIN_STRING , "%d" , 
			   &Bulletmap[BulletIndex].damage);

      // Now we read in the number of phases that are designed for this bullet type
      // THIS IS NOW SPECIFIED IN THE THEME CONFIG FILE
      // ReadValueFromString( BulletPointer ,  BULLET_NUMBER_OF_PHASES_BEGIN_STRING , "%d" , 
      // &Bulletmap[BulletIndex].phases , EndOfBulletData );

      // Now we read in the type of blast this bullet will cause when crashing e.g. against the wall
      ReadValueFromString( BulletPointer ,  BULLET_BLAST_TYPE_CAUSED_BEGIN_STRING , "%d" , 
			   &Bulletmap[BulletIndex].blast);
 
      BulletIndex++;
    }

  //--------------------
  // Now that the detailed values for the bullets have been read in,
  // we now read in the general calibration contants and after that
  // the start to apply them right now, so they also take effect.
  
  DebugPrintf (1, "\n\nStarting to read bullet calibration section\n\n");

  // Now we read in the speed calibration factor for all bullets
  ReadValueFromString( DataPointer ,  BULLET_SPEED_CALIBRATOR_STRING , "%f" , 
		       &bullet_speed_calibrator);

  // Now we read in the damage calibration factor for all bullets
  ReadValueFromString( DataPointer ,  BULLET_DAMAGE_CALIBRATOR_STRING , "%f" , 
		       &bullet_damage_calibrator);

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
  int i, size;

  float maxspeed_calibrator;
  float acceleration_calibrator;
  float maxenergy_calibrator;
  float energyloss_calibrator;
  float aggression_calibrator;
  float score_calibrator;

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
#define ADVANCED_FIGHTING_BEGIN_STRING "Advanced Fighting present in this droid : "
#define GO_REQUEST_REINFORCEMENTS_BEGIN_STRING "Going to request reinforcements typical for this droid : "
#define NOTES_BEGIN_STRING "Notes concerning this droid : "

  
  RobotPointer = LocateStringInData ( DataPointer , ROBOT_SECTION_BEGIN_STRING );
  EndOfDataPointer = LocateStringInData ( DataPointer , ROBOT_SECTION_END_STRING );

  
  DebugPrintf (2, "\n\nStarting to read robot calibration section\n\n");

  // Now we read in the speed calibration factor for all droids
  ReadValueFromString( RobotPointer , MAXSPEED_CALIBRATOR_STRING , "%f" , 
		       &maxspeed_calibrator);

  // Now we read in the acceleration calibration factor for all droids
  ReadValueFromString( RobotPointer , ACCELERATION_CALIBRATOR_STRING , "%f" , 
		       &acceleration_calibrator);

  // Now we read in the maxenergy calibration factor for all droids
  ReadValueFromString( RobotPointer , MAXENERGY_CALIBRATOR_STRING , "%f" , 
		       &maxenergy_calibrator);

  // Now we read in the energy_loss calibration factor for all droids
  ReadValueFromString( RobotPointer , ENERGYLOSS_CALIBRATOR_STRING , "%f" , 
		       &energyloss_calibrator);

  // Now we read in the aggression calibration factor for all droids
  ReadValueFromString( RobotPointer , AGGRESSION_CALIBRATOR_STRING , "%f" , 
		       &aggression_calibrator);

  // Now we read in the score calibration factor for all droids
  ReadValueFromString( RobotPointer , SCORE_CALIBRATOR_STRING , "%f" , 
		       &score_calibrator);

  DebugPrintf ( 1 , "\n\nStarting to read Robot data...\n\n" );
  
  // cleanup if previously allocated:
  if (Number_Of_Droid_Types && Druidmap)
    {
      for (i=0; i< Number_Of_Droid_Types; i++)
	if (Druidmap[i].notes) {
	  free (Druidmap[i].notes);
	  Druidmap[i].notes = NULL;
	}

      free(Druidmap);
      Druidmap = NULL;
    }
    
  //--------------------
  // At first, we must allocate memory for the droid specifications.
  // How much?  That depends on the number of droids defined in freedroid.ruleset.
  // So we have to count those first.  ok.  lets do it.
  Number_Of_Droid_Types = CountStringOccurences ( DataPointer , NEW_ROBOT_BEGIN_STRING ) ;

  // Now that we know how many robots are defined in freedroid.ruleset, we can allocate
  // a fitting amount of memory.
  size = sizeof(druidspec);
  Druidmap = MyMalloc ( size * (Number_Of_Droid_Types + 1) + 1 );
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
      ReadValueFromString (RobotPointer, DROIDNAME_BEGIN_STRING, "%s", Druidmap[RobotIndex].druidname);

      // Now we read in the maximal speed this droid can go. 
      ReadValueFromString( RobotPointer , MAXSPEED_BEGIN_STRING , "%f" , 
			   &Druidmap[RobotIndex].maxspeed);

      // Now we read in the class of this droid.
      ReadValueFromString( RobotPointer , CLASS_BEGIN_STRING , "%d" , 
			   &Druidmap[RobotIndex].class);

      // Now we read in the maximal acceleration this droid can go. 
      ReadValueFromString( RobotPointer , ACCELERATION_BEGIN_STRING , "%f" , 
			   &Druidmap[RobotIndex].accel);

      // Now we read in the maximal energy this droid can store. 
      ReadValueFromString( RobotPointer , MAXENERGY_BEGIN_STRING , "%f" , 
			   &Druidmap[RobotIndex].maxenergy);

      // Now we read in the lose_health rate.
      ReadValueFromString( RobotPointer , LOSEHEALTH_BEGIN_STRING , "%f" , 
			   &Druidmap[RobotIndex].lose_health);

      // Now we read in the class of this droid.
      ReadValueFromString( RobotPointer , GUN_BEGIN_STRING , "%d" , 
			   &Druidmap[RobotIndex].gun);

      // Now we read in the aggression rate of this droid.
      ReadValueFromString( RobotPointer , AGGRESSION_BEGIN_STRING , "%d" , 
			   &Druidmap[RobotIndex].aggression);

      // Now we read in the flash immunity of this droid.
      ReadValueFromString( RobotPointer , FLASHIMMUNE_BEGIN_STRING , "%d" , 
			   &Druidmap[RobotIndex].flashimmune);

      // Now we score to be had for destroying one droid of this type
      ReadValueFromString( RobotPointer , SCORE_BEGIN_STRING , "%d" , 
			   &Druidmap[RobotIndex].score);

      // Now we read in the height of this droid of this type
      ReadValueFromString( RobotPointer , HEIGHT_BEGIN_STRING , "%f" , 
			   &Druidmap[RobotIndex].height);

      // Now we read in the weight of this droid type
      ReadValueFromString( RobotPointer , WEIGHT_BEGIN_STRING , "%d" , 
			   &Druidmap[RobotIndex].weight);

      // Now we read in the drive of this droid of this type
      ReadValueFromString( RobotPointer , DRIVE_BEGIN_STRING , "%d" , 
			   &Druidmap[RobotIndex].drive);

      // Now we read in the brain of this droid of this type
      ReadValueFromString( RobotPointer , BRAIN_BEGIN_STRING , "%d" , 
			   &Druidmap[RobotIndex].brain);

      // Now we read in the sensor 1, 2 and 3 of this droid type
      ReadValueFromString( RobotPointer , SENSOR1_BEGIN_STRING , "%d" , 
			   &Druidmap[RobotIndex].sensor1);
      ReadValueFromString( RobotPointer , SENSOR2_BEGIN_STRING , "%d" , 
			   &Druidmap[RobotIndex].sensor2);
      ReadValueFromString( RobotPointer , SENSOR3_BEGIN_STRING , "%d" , 
			   &Druidmap[RobotIndex].sensor3);

      // Now we read in the notes concerning this droid.  We consider as notes all the rest of the
      // line after the NOTES_BEGIN_STRING until the "\n" is found.
      Druidmap[RobotIndex].notes = ReadAndMallocStringFromData (RobotPointer, NOTES_BEGIN_STRING, "\n");

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
  fpath = find_file (Datafilename, MAP_DIR, NO_THEME, CRITICAL);

  Data = ReadAndMallocAndTerminateFile( fpath , END_OF_GAME_DAT_STRING ) ;

  Get_General_Game_Constants( Data );

  Get_Robot_Data ( Data );

  Get_Bullet_Data ( Data );

  //--------------------
  // Now we read in the total time amount for the blast animations
#define BLAST_ONE_TOTAL_AMOUNT_OF_TIME_STRING "Time in seconds for the animation of blast one :"
#define BLAST_TWO_TOTAL_AMOUNT_OF_TIME_STRING "Time in seconds for the animation of blast one :"

  ReadValueFromString (Data, BLAST_ONE_TOTAL_AMOUNT_OF_TIME_STRING, "%f", &Blastmap[0].total_animation_time);
  ReadValueFromString (Data, BLAST_TWO_TOTAL_AMOUNT_OF_TIME_STRING, "%f", &Blastmap[1].total_animation_time);


  free ( Data ); 
  
  return;

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

char copyright[] = "\nCopyright (C) 2003 Johannes Prix, Reinhard Prix\n\
Freedroid comes with NO WARRANTY to the extent permitted by law.\n\
You may redistribute copies of Freedroid under the terms of the\n\
GNU General Public License.\n\
For more information about these matters, see the file named COPYING.\n";


char usage_string[] =
  "Usage: freedroid [-v|--version] \n\
                    [-q|--nosound] \n\
                    [-s|--sound] \n\
                    [-f|--fullscreen] [-w|--window]\n\
                    [-j|--sensitivity]\n\
                    [-d|--debug=LEVEL]\n\
                    [-r|--scale=RESOLUTION_SCALE]\n\
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
    {"version", 	0, 0, 'v'},
    {"help", 		0, 0, 'h'},
    {"nosound", 	0, 0, 'q'},
    {"sound", 		0, 0, 's'},
    {"debug", 		2, 0, 'd'},
    {"window",  	0, 0, 'w'},
    {"fullscreen",	0, 0, 'f'},
    {"sensitivity",	1, 0, 'j'},
    {"scale",		1, 0, 'r'},
    { 0, 		0, 0,  0}
  };

  //   sound_on=TRUE;

  while (1)
    {
      c = getopt_long (argc, argv, "vqst:h?d::wfj:r:", long_options, NULL);
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

	case 'r':
	  GameConfig.scale = (float)atof (optarg);
	  if (GameConfig.scale == 0)
	    {
	      DebugPrintf (0, "ERROR: illegal scale entered: %s\n", optarg);
	      Terminate (ERR);
	    }
	  DebugPrintf (1, "Graphics scale set to %f\n", GameConfig.scale);
	  break;

	case 'f':
	  GameConfig.UseFullscreen = TRUE;
	  break;
	case 'w':
	  GameConfig.UseFullscreen = FALSE;
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
  char *StartPointPointer;
  char Buffer[500];
  int NumberOfStartPoints=0;
  int RealStartPoint=0;
  int StartingLevel=0;
  int StartingXPos=0;
  int StartingYPos=0;
  BFont_Info *oldfont;

#define END_OF_MISSION_DATA_STRING "*** End of Mission File ***"
#define MISSION_BRIEFING_BEGIN_STRING "** Start of Mission Briefing Text Section **"
#define MISSION_ENDTITLE_SONG_NAME_STRING "Song name to play in the end title if the mission is completed: "
#define SHIPNAME_INDICATION_STRING "Ship file to use for this mission: "
#define ELEVATORNAME_INDICATION_STRING "Lift file to use for this mission: "
#define CREWNAME_INDICATION_STRING "Crew file to use for this mission: "
#define GAMEDATANAME_INDICATION_STRING "Physics ('game.dat') file to use for this mission: "
#define MISSION_ENDTITLE_BEGIN_STRING "** Beginning of End Title Text Section **"
#define MISSION_ENDTITLE_END_STRING "** End of End Title Text Section **"
#define MISSION_START_POINT_STRING "Possible Start Point : "

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
  LastGotIntoBlastSound = 2;
  LastRefreshSound = 2;
  ThisMessageTime = 0;
  LevelDoorsNotMovedTime = 0.0;
  DeathCount = 0;

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
  for (i=0; i < MAX_ENEMYS_ON_SHIP; i++)
    {
      AllEnemys[i].type = OUT;
      AllEnemys[i].energy = -1;
    }
  DebugPrintf (2, "\nvoid InitNewMission( ... ): All enemys have been deleted...");
  
  //--------------------
  //Now its time to start decoding the mission file.
  //For that, we must get it into memory first.
  //The procedure is the same as with LoadShip

  oldfont = GetCurrentFont ();

  SetCurrentFont (FPS_Display_BFont);
  //  printf_SDL (ne_screen, User_Rect.x + 50, -1, "Loading mission data ");

  /* Read the whole mission data to memory */
  fpath = find_file (MissionName, MAP_DIR, NO_THEME, CRITICAL);

  MainMissionPointer = ReadAndMallocAndTerminateFile( fpath , END_OF_MISSION_DATA_STRING ) ;

  //--------------------
  // Now the mission file is read into memory.  That means we can start to decode the details given
  // in the body of the mission file.  

  //--------------------
  // First we extract the game physics file name from the
  // mission file and load the game data.
  //
  ReadValueFromString (MainMissionPointer, GAMEDATANAME_INDICATION_STRING, "%s", Buffer);

  Init_Game_Data (Buffer);

  //--------------------
  // Now its time to get the shipname from the mission file and
  // read the ship file into the right memory structures
  //
  ReadValueFromString ( MainMissionPointer, SHIPNAME_INDICATION_STRING, "%s", Buffer);

  if ( LoadShip (Buffer) == ERR )
    {
      DebugPrintf (0, "Error in LoadShip\n");
      Terminate (ERR);
    }
  //--------------------
  // Now its time to get the elevator file name from the mission file and
  // read the elevator file into the right memory structures
  //
  ReadValueFromString (MainMissionPointer, ELEVATORNAME_INDICATION_STRING, "%s", Buffer);

  if (GetLiftConnections (Buffer) == ERR)
    {
      DebugPrintf (1, "\nError in GetLiftConnections ");
      Terminate (ERR);
    }
  //  printf_SDL (ne_screen, -1, -1, ".");
  //--------------------
  // We also load the comment for the influencer to say at the beginning of the mission
  //

  // NO! these strings are allocated elsewhere or even static, so free'ing them
  // here would SegFault eventually!
  //  if (Me.TextToBeDisplayed) free (Me.TextToBeDisplayed);

  Me.TextToBeDisplayed =
    ReadAndMallocStringFromData ( MainMissionPointer , "Influs mission start comment=\"" , "\"" ) ;
  Me.TextVisibleTime = 0;


  //--------------------
  // Now its time to get the crew file name from the mission file and
  // assemble an appropriate crew out of it
  //
  ReadValueFromString (MainMissionPointer, CREWNAME_INDICATION_STRING, "%s", Buffer);

  /* initialize enemys according to crew file */
  // WARNING!! THIS REQUIRES THE freedroid.ruleset FILE TO BE READ ALREADY, BECAUSE
  // ROBOT SPECIFICATIONS ARE ALREADY REQUIRED HERE!!!!!
  if (GetCrew (Buffer) == ERR)
    {
      DebugPrintf (1, "\nInitNewGame(): ERROR: Initialization of enemys failed...");
      Terminate (-1);
    }

  //--------------------
  // Now its time to get the debriefing text from the mission file so that it
  // can be used, if the mission is completed and also the end title music name
  // must be read in as well
  ReadValueFromString (MainMissionPointer, MISSION_ENDTITLE_SONG_NAME_STRING, "%s", DebriefingSong);

  if (DebriefingText) free(DebriefingText);
  DebriefingText =
    ReadAndMallocStringFromData (MainMissionPointer, MISSION_ENDTITLE_BEGIN_STRING, MISSION_ENDTITLE_END_STRING);

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
  DebugPrintf (1, "\nFound %d different starting points for the mission in the mission file.", 
	       NumberOfStartPoints );


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
  DebugPrintf ( 1 , "\nFinal starting position: Level=%d XPos=%d YPos=%d." , 
		StartingLevel, StartingXPos, StartingYPos );


  /* Reactivate the light on alle Levels, that might have been dark */
  for (i = 0; i < curShip.num_levels; i++)
    curShip.AllLevels[i]->empty = FALSE;

  DebugPrintf (2, "\nvoid InitNewMission( ... ): All levels have been set to 'active'...");


  //--------------------
  // At this point the position history can be initialized
  //
  InitInfluPositionHistory();
  //  printf_SDL (ne_screen, -1, -1, ".");


  //  printf_SDL (ne_screen, -1, -1, " ok\n");
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

  // Switch_Background_Music_To (COMBAT_BACKGROUND_MUSIC_SOUND);
  Switch_Background_Music_To ( CurLevel->Background_Song_Name );

  for (i = 0; i < curShip.num_levels; i++)
    {
      CurLevel = curShip.AllLevels[i];
      ShuffleEnemys ();
    }
  CurLevel = curShip.AllLevels[ StartingLevel ];

  // Now that the briefing and all that is done,
  // the influence structure can be initialized for
  // the new mission:
  Me.type = DRUID001;
  Me.speed.x = 0;
  Me.speed.y = 0;
  Me.energy = Druidmap[DRUID001].maxenergy;
  Me.health = Me.energy;	/* start with max. health */
  Me.status = MOBILE;
  Me.phase = 0;
  Me.timer = 0.0;  // set clock to 0

  DebugPrintf (1, "done."); // this matches the printf at the beginning of this function

  free (MainMissionPointer);
 
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
InitFreedroid (int argc, char *const argv[])
{
  int i;

  Bulletmap=NULL;  // That will cause the memory to be allocated later

  for ( i = 0 ; i < MAXBULLETS ; i++ )
    AllBullets[i].Surfaces_were_generated = FALSE;

  SkipAFewFrames = FALSE;
  Me.TextVisibleTime = 0;
  Me.TextToBeDisplayed = NULL;

  // these are the hardcoded game-defaults, they can be overloaded by the config-file if present
  GameConfig.Current_BG_Music_Volume=0.3;
  GameConfig.Current_Sound_FX_Volume=0.5;

  GameConfig.WantedTextVisibleTime = 3;
  GameConfig.Droid_Talk = FALSE;

  GameConfig.Draw_Framerate = TRUE;
  GameConfig.Draw_Energy = TRUE;
  GameConfig.Draw_DeathCount = FALSE;
  GameConfig.Draw_Position = FALSE;

  sprintf (GameConfig.Theme_Name, "classic");
  GameConfig.FullUserRect = TRUE;
  GameConfig.UseFullscreen = FALSE;
  GameConfig.TakeoverActivates = TRUE;  
  GameConfig.ShowDecals = TRUE;
  GameConfig.AllMapVisible = TRUE;    // classic setting: map always visible
  GameConfig.scale = 1.0;  	 // overall scaling of _all_ graphics (e.g. for 320x200 displays)

  // now load saved options from the config-file
  LoadGameConfig ();

  // call this _after_ default settings and LoadGameConfig() ==> cmdline has highest priority!
  parse_command_line (argc, argv);

  if (GameConfig.FullUserRect)
    Copy_Rect(Full_User_Rect, User_Rect);
  else
    Copy_Rect(Classic_User_Rect, User_Rect);


  FindAllThemes ();  // put all found themes into a list: AllThemes[]

  ScaleRect (Screen_Rect, GameConfig.scale);   // make sure we open a window of the right (rescaled) size!
  Init_Video ();

  DisplayImage (find_file (TITLE_PIC_FILE, GRAPHICS_DIR, NO_THEME, CRITICAL)); // show title pic
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

  /* initialize/load the highscore list */
  InitHighscores ();
 
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
  char Buffer[500];
  int ThisTextLength;
  SDL_Rect rect;
#define BRIEFING_TITLE_PICTURE_STRING "The title picture in the graphics subdirectory for this mission is : "
#define BRIEFING_TITLE_SONG_STRING "The title song in the sound subdirectory for this mission is : "
#define NEXT_BRIEFING_SUBSECTION_START_STRING "* New Mission Briefing Text Subsection *"
#define END_OF_BRIEFING_SUBSECTION_STRING "* End of Mission Briefing Text Subsection *"

  ReadValueFromString (MissionBriefingPointer, BRIEFING_TITLE_SONG_STRING, "%s", Buffer);
  Switch_Background_Music_To (Buffer);

  SDL_SetClipRect ( ne_screen, NULL );
  ReadValueFromString (MissionBriefingPointer, BRIEFING_TITLE_PICTURE_STRING, "%s", Buffer);
  DisplayImage ( find_file(Buffer, GRAPHICS_DIR, NO_THEME, CRITICAL) );
  MakeGridOnScreen( (SDL_Rect*) &Screen_Rect );
  Me.status=BRIEFING;
  //  SDL_Flip (ne_screen);

  SetCurrentFont( Para_BFont );

  DisplayBanner (NULL, NULL,  BANNER_FORCE_UPDATE ); 

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
      if (ScrollText ( PreparedBriefingText, &rect , 0 ) == 1)
	break;  // User pressed 'fire'
 
      free ( PreparedBriefingText );
    }

  return;

} /* Title() */

/*----------------------------------------------------------------------*/
/* $Function----------------------------------------------------------*/
void
ThouArtVictorious(void)
{
  SDL_Rect rect;
  Uint32 now;

  Switch_Background_Music_To ( DebriefingSong );

  SDL_ShowCursor (SDL_DISABLE);

  ShowScore = (long)RealScore;
  Me.status = VICTORY;
  DisplayBanner (NULL, NULL,  BANNER_FORCE_UPDATE );

  // release fire
  if (FirePressedR());

  now=SDL_GetTicks();

  while ( (SDL_GetTicks() - now < WAIT_AFTER_KILLED) )
    {
      DisplayBanner (NULL, NULL,  0 );
      ExplodeBlasts ();
      MoveBullets ();
      Assemble_Combat_Picture ( DO_SCREEN_UPDATE );
    }

  Copy_Rect(Full_User_Rect, rect);
  SDL_SetClipRect ( ne_screen, NULL );
  MakeGridOnScreen (&rect);
  SDL_Flip(ne_screen);
  rect.x += 10;
  rect.w -= 20;  //leave some border
  SetCurrentFont( Para_BFont);
  ScrollText (DebriefingText , &rect , 6 );

  // release fire
  if (FirePressedR());

  return;
} 

/*@Function============================================================
@Desc: Show end-screen 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
ThouArtDefeated (void)
{
  Uint32 now, delay;
  SDL_Rect dst;
  int h;

  Me.status = TERMINATED;
  SDL_ShowCursor (SDL_DISABLE);

  ExplodeInfluencer ();

  now = SDL_GetTicks();

  while ( (delay=SDL_GetTicks() - now) < WAIT_AFTER_KILLED)
    {
      // bit of a dirty hack:  get "slow motion effect" by fiddlig with FPSover1
      FPSover1 *= 2.0;

      StartTakingTimeForFPSCalculation();
      DisplayBanner (NULL, NULL,  0 );
      ExplodeBlasts ();
      MoveBullets ();
      MoveEnemys ();
      Assemble_Combat_Picture ( DO_SCREEN_UPDATE );
      ComputeFPSForThisFrame ();
    }
  
#ifdef HAVE_LIBSDL_MIXER
  Mix_HaltMusic ();
#endif

  // important!!: don't forget to stop fps calculation here (bugfix: enemy piles after gameOver)
  Activate_Conservative_Frame_Computation ();

  white_noise (ne_screen, &User_Rect, WAIT_AFTER_KILLED);

  Assemble_Combat_Picture (DO_SCREEN_UPDATE);
  MakeGridOnScreen (&User_Rect);

  Set_Rect (dst, UserCenter_x - Portrait_Rect.w/2, UserCenter_y - Portrait_Rect.h/2, 
	    Portrait_Rect.w, Portrait_Rect.h);
  SDL_BlitSurface (pic999, NULL, ne_screen, &dst);
  ThouArtDefeatedSound ();

  SetCurrentFont (Para_BFont);
  h = FontHeight (Para_BFont);
  DisplayText ("Transmission", dst.x - h, dst.y - h, &User_Rect);
  DisplayText ("Terminated",  dst.x -h, dst.y + dst.h, &User_Rect);
  printf_SDL(ne_screen, -1, -1, "\n");
  SDL_Flip (ne_screen);
  
  now = SDL_GetTicks ();

  while (SDL_GetTicks() - now < SHOW_WAIT) SDL_Delay(1);

  UpdateHighscores ();

  GameOver = TRUE;

  return;
} // void ThouArtDefeated(void)

/*----------------------------------------------------------------------
 * This function checks, if the influencer has succeeded in his given 
 * mission.  If not it returns, if yes the Debriefing is
 * started.
 ----------------------------------------------------------------------*/
void 
CheckIfMissionIsComplete (void)
{
  int i;

  for ( i=0 ; i < NumEnemys ; i++ )
    if ( (AllEnemys[i].status != OUT) && (AllEnemys[i].status != TERMINATED) )
      return;

  // mission complete: all droids have been killed
  RealScore += MISSION_COMPLETE_BONUS;

  ThouArtVictorious();

  //  UpdateHighscores();

  GameOver = TRUE;
  
} // void CheckIfMissionIsComplete

// ----------------------------------------------------------------------
// find all themes and put them in AllThemes
// ----------------------------------------------------------------------
void
FindAllThemes (void)
{
  int i, location;
  char dname[500], tname[100], fpath[500];
  DIR *dir;
  struct stat buf;
  struct dirent *entry;
  char *pos;
  int len;
  FILE *fp;

  // just to make sure...
  AllThemes.num_themes = 0;
  for (i=0; i< MAX_THEMES; i++)
    {
      if (AllThemes.theme_name[i]) free(AllThemes.theme_name[i]);
      AllThemes.theme_name[i] = NULL;
    }

  for (location=0; location < 2; location++)
    {
      if (location == 0)
	strcpy (dname, FD_DATADIR);   /* first scan FD_DATADIR */
      if (location == 1)
	strcpy (dname, ".."); /* then the local graphics-dir */

      strcat (dname, "/graphics");

      if ( (dir = opendir(dname)) == NULL)
	{
	  DebugPrintf (1, "WARNING: can't open data-directory %s...\n", dname);
	  continue;
	}

      while ( (entry = readdir (dir)) != NULL )
	{
	  strcpy (fpath, dname);
	  strcat (fpath, "/");
	  strcat (fpath, entry->d_name);

	  if ( stat(fpath, &buf) != 0 ) // error stat'ing the file
	    {
	      DebugPrintf (1, "WARNING: could non stat %s!\n", fpath);
	      continue;
	    }
	  if ( ! S_ISDIR( buf.st_mode ) )  // is it a directory
	    continue;

	  if ( (pos = strstr (entry->d_name, "_theme")) == NULL)   // does its name contain "_theme"
	    continue;
	  if ( *(pos+strlen("_theme")) != 0 )           // is it *_theme ?
	    continue;

	  // yes!! -> found a possible theme-dir
	  len = strlen(entry->d_name)-strlen("_theme");
	  if (len >= 100)
	    {
	      DebugPrintf (0, "WARNING: theme-name of '%s' longer than allowed 100 chars... discarded!\n",
			   entry->d_name);
	      continue;
	    }
	  strncpy (tname, entry->d_name, len);
	  tname[len]= '\0';  // null-terminate!
	  DebugPrintf (1, "Hmm, seems we found a new theme: %s\n", tname);
	  // check readabiltiy of "config.theme"
	  sprintf (fpath, "%s/%s_theme/config.theme", dname, tname);
	  if ( (fp = fopen (fpath, "r")) != NULL)  
	    { // config.theme is readable
	      fclose(fp);
	      DebugPrintf (1, "..and config.theme seems readable ... good! \n");
	      // last check: is this theme already in the list??
	      for (i=0; i< AllThemes.num_themes; i++)
		if ( strcmp(tname, AllThemes.theme_name[i])== 0)
		  break;
	      if ( i < AllThemes.num_themes ) 
		{ // found it in the list already
		  DebugPrintf (1, "Theme '%s' is already listed ... \n", tname);
		  continue;
		}
	      else
		{ // not yet listed --> we found a new theme!
		  DebugPrintf (0, "Found new graphics-theme: %s \n", tname);
		  AllThemes.theme_name[AllThemes.num_themes] = MyMalloc (strlen(tname)+1);
		  strcpy (AllThemes.theme_name[AllThemes.num_themes], tname);
		  AllThemes.num_themes ++;
		}
	    }
	  else
	    { // config.theme not readable
	      DebugPrintf (0, "WARNING: config.theme of theme '%s' not readable -> discarded!\n", tname);
	      continue;
	    }

	} // while more directory entries

      closedir (dir);

    } /* for all data-dir locations */

  // now have a look at what we found:
  if (AllThemes.num_themes == 0)
    {
      DebugPrintf (0, "ERROR: no valid graphic-themes found!! \n");
      DebugPrintf (0, "You need to install at least one to run Freedroid!!\n");
      Terminate (ERR);
    }
  
  for (i=0; i< AllThemes.num_themes; i++)
    if ( strcmp (AllThemes.theme_name[i], GameConfig.Theme_Name) == 0)
      break;
  if (i >= AllThemes.num_themes)
    {
      DebugPrintf (0, "WARNING: selected theme %s not valid! Using first theme in list.\n",
		   GameConfig.Theme_Name);
      strcpy (GameConfig.Theme_Name, AllThemes.theme_name[0]);
      AllThemes.cur_tnum = 0;
    }
  else
    AllThemes.cur_tnum = i;

  DebugPrintf (0, "Game starts using theme: %s\n", GameConfig.Theme_Name);

  return;

} // FindAllThemes

/*----------------------------------------------------------------------
 * put some ideology message for our poor friends enslaved by M$-Win32 ;)
 *----------------------------------------------------------------------*/
void
Win32Disclaimer (void)
{
  SDL_Rect rect;

  SDL_SetClipRect ( ne_screen, NULL );
  DisplayImage (find_file (TITLE_PIC_FILE, GRAPHICS_DIR, NO_THEME, CRITICAL)); // show title pic
  MakeGridOnScreen( (SDL_Rect*) &Screen_Rect );

  SetCurrentFont( Para_BFont );

  Copy_Rect(Full_User_Rect, rect);
  rect.x += 10;
  rect.w -= 10; //leave some border
  DisplayText ("Windows disclaimer:\n\nThis program is 100% Free (as in Freedom), licenced under the GPL.\n\
It is developed on a free operating system (GNU/Linux) using exclusively free tools. \
For more information about Free Software see the GPL licence (in the file COPYING)\n\
or visit http://www.gnu.org.\n\n\n Press fire to play.", rect.x, rect.y, &rect);
  SDL_Flip (ne_screen);
  Wait4Fire();

  return;
}


#undef _init_c
