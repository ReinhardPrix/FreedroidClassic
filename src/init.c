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

void 
Get_Bullet_Data ( char* DataPointer )
{
  char *BulletPointer;
  char *ValuePointer;  // we use ValuePointer while RobotPointer stays still to allow for
                       // interchanging of the order of appearance of parameters in the game.dat file
  int i;

  double bullet_speed_calibrator;
  double bullet_damage_calibrator;

#define BULLET_SECTION_BEGIN_STRING "*** Start of Bullet Data Section: ***" 
#define BULLET_SECTION_END_STRING "*** End of Bullet Data Section: ***" 
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
  
  DebugPrintf (2, "\n\nStarting to read bullet calibration section\n\n");

  // Now we read in the speed calibration factor for all bullets
  if ( (ValuePointer = strstr ( BulletPointer, BULLET_SPEED_CALIBRATOR_STRING )) == NULL )
    {
      DebugPrintf(1, "\nERROR! NO BULLET SPEED CALIBRATOR ENTRY FOUND! TERMINATING!");
      Terminate(ERR);
    }
  else
    {
      ValuePointer += strlen ( BULLET_SPEED_CALIBRATOR_STRING );
      sscanf ( ValuePointer , "%lf" , &bullet_speed_calibrator );
    }

  // Now we read in the damage calibration factor for all bullets
  if ( (ValuePointer = strstr ( BulletPointer, BULLET_DAMAGE_CALIBRATOR_STRING )) == NULL )
    {
      DebugPrintf(1, "\nERROR! NO BULLET DAMAGE CALIBRATOR ENTRY FOUND! TERMINATING!");
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
  for ( i = 0 ; i < ALLBULLETTYPES ; i++ )
    {
      Bulletmap[i].speed *= bullet_speed_calibrator;
      Bulletmap[i].damage *= bullet_damage_calibrator;
    }

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
  Druidmap = malloc ( i * (Number_Of_Droid_Types + 1) + 1 );
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
	  Druidmap[RobotIndex].druidname = malloc ( StringLength + 1 );
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
	  sscanf ( ValuePointer , "%d" , &Druidmap[RobotIndex].AdvancedFighting );
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
	  Druidmap[RobotIndex].notes = malloc ( StringLength + 1 );
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


} // int Get_Robot_Data ( void )

/*@Function============================================================
@Desc: This function loads all the constant variables of the game from
       a dat file, that should be optimally human readable.

@Ret: 
* $Function----------------------------------------------------------*/
void
Init_Game_Data ( char * Datafilename )
{
  struct stat stbuf;
  FILE *DataFile;
  char *Data;
  char *EndPointer;
  // char filename[10000];
#define END_OF_GAME_DAT_STRING "*** End of game.dat File ***"

  // strcpy ( filename , MAP_DIR );
  // strcat ( filename , Datafilename );

  DebugPrintf (2, "\nint Init_Game_Data ( char* Datafilename ) called.");
  // printf("\nint Init_Game_Data ( char* Datafilename ): The filename is: %s" , Datafilename );

  /* Read the whole game data to memory */
  if ((DataFile = fopen (Datafilename, "r")) == NULL)
    {
      DebugPrintf (1, "\nint Init_Game_Data( void ): Error opening file.... ");
      Terminate(ERR);
    }
  else
    {
      DebugPrintf (2, "\nOpening game data file succeeded...");
    }

  if (fstat (fileno (DataFile), &stbuf) == EOF)
    {
      DebugPrintf (1, "\nint Init_Game_Data ( void ): Error fstat-ing File....");
      Terminate(ERR);
    }
  else
    {
      DebugPrintf (2, "\nfstating game data file succeeded...");
    }

  if ((Data = (char *) malloc (stbuf.st_size + 64*2)) == NULL)
    {
      DebugPrintf (1, "\nint Init_Game_Data ( char * constantsname ) : Out of Memory? ");
      Terminate(ERR);
    }

  fread ( Data, (size_t) 64, (size_t) (stbuf.st_size / 64 +1 ), DataFile);

  DebugPrintf (2, "\nReading dat file succeeded... Adding a 0 at the end of read data....");

  if ( (EndPointer = strstr( Data , END_OF_GAME_DAT_STRING ) ) == NULL )
    {
      DebugPrintf (1, "\nERROR!  END OF GAME.DAT STRING NOT FOUND!  Terminating...");
      Terminate(ERR);
    }
  else
    {
      EndPointer[0]=0; // we want to handle the file like a string, even if it is not zero
                       // terminated by nature.  We just have to add the zero termination.
    }

  // printf("\n\nvoid Init_Game_Data: The content of the read file: \n%s" , Data );

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
                    [-t|--timeout=SECONDS] \n\
                    [-f|--fullscreen] [-w|--window]\n\
                    [-j|--sensitivity]\n\
                    [-d|--debug=LEVEL]\n\
\n\
Report bugs to freedroid@??? (sorry, havent got one yet ;)\n";

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
  int i;
  struct stat stbuf;
  FILE *MissionFile;
  char *MainMissionPointer;
  char *BriefingSectionPointer;
  char *EndPointer;
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
  // char filename[]=MAP_DIR "game.dat";
  // #define END_OF_GAME_DAT_STRING "*** End of game.dat File ***"

#define END_OF_MISSION_DATA_STRING "*** End of Mission File ***"
#define MISSION_BRIEFING_BEGIN_STRING "** Start of Mission Briefing Text Section **"
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
  Draw_Framerate=TRUE;
  // Draw_Energy=FALSE;
  Draw_Energy=TRUE;
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
  if ((MissionFile = fopen ( MissionName , "r")) == NULL)
    {
      DebugPrintf (2, "\nint InitNewMission( ... ): Error opening file.... ");
      Terminate(ERR);
    }
  else
    {
      // printf("\nOpening %s file succeeded..." , MissionName );
    }

  if (fstat (fileno (MissionFile), &stbuf) == EOF)
    {
      DebugPrintf (1, "\nint InitNewMission ( void ): Error fstat-ing File....");
      Terminate(ERR);
    }
  else
    {
      // printf("\nfstating %s file succeeded..." , MissionName );
    }

  if (( MainMissionPointer = (char *) malloc (stbuf.st_size + 64*2)) == NULL)
    {
      DebugPrintf (2, "\nint InitNewMission ( char * constantsname ) : Out of Memory? ");
      Terminate(ERR);
    }

  fread ( MainMissionPointer , (size_t) 64, (size_t) (stbuf.st_size / 64 +1 ), MissionFile);

  DebugPrintf (2, "\nReading dat file succeeded... Adding a 0 at the end of read data....");

  if ( (EndPointer = strstr( MainMissionPointer , END_OF_MISSION_DATA_STRING ) ) == NULL )
    {
      DebugPrintf (1, "\nERROR!  END OF MISSION DATA STRING NOT FOUND!  Terminating...");
      Terminate(ERR);
    }
  else
    {
      EndPointer[0]=0; // we want to handle the file like a string, even if it is not zero
                       // terminated by nature.  We just have to add the zero termination.
    }

  // printf("\n\nvoid InitNewMission: The content of the read file: \n%s" , MainMissionPointer );

  //--------------------
  //Now the mission file is read into memory
  //That means we can start to decode the details given
  //in the body of the mission file.  We start with 
  //doing the briefing things...

  // First we search for the beginning of the mission briefing big section NOT subsection
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
  DebugPrintf (2, "\nvoid InitNewGame(void): The title signaton has been successfully displayed...:");

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
      strcpy( GameDataName , MAP_DIR );
      GameDataNamePointer += strlen ( GAMEDATANAME_INDICATION_STRING );
      
      GameDataNameLength = strstr ( GameDataNamePointer , "\n") - GameDataNamePointer;
      strncat( GameDataName , GameDataNamePointer , GameDataNameLength );
      GameDataName[ strlen(MAP_DIR) + GameDataNameLength ] = 0;
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
      strcpy( Shipname , MAP_DIR );
      ShipnamePointer += strlen ( SHIPNAME_INDICATION_STRING );
      
      ShipnameLength = strstr (ShipnamePointer , "\n") - ShipnamePointer;
      strncat( Shipname , ShipnamePointer , ShipnameLength );
      Shipname[ strlen(MAP_DIR) + ShipnameLength ] = 0;
      
      
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
      strcpy( Liftname , MAP_DIR );
      LiftnamePointer += strlen ( ELEVATORNAME_INDICATION_STRING );
      
      LiftnameLength = strstr (LiftnamePointer , "\n") - LiftnamePointer;
      strncat( Liftname , LiftnamePointer , LiftnameLength );
      Shipname[ strlen(MAP_DIR) + LiftnameLength ] = 0;
      
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
      ship_on_filename = malloc(2000);
      strcpy( ship_on_filename , GRAPHICS_DIR );
      ShipOnPointer += strlen ( LIFTS_ON_INDICATION_STRING );
      
      ShipOnLength = strstr ( ShipOnPointer , "\n") - ShipOnPointer;
      strncat( ship_on_filename , ShipOnPointer , ShipOnLength );
      ship_on_filename[ strlen(GRAPHICS_DIR) + ShipOnLength ] = 0;
      
      DebugPrintf (1, "\nShipOn file name found!  It reads: %s" , ship_on_filename );
    }
  if ( ( ShipOffPointer = strstr ( MainMissionPointer, LIFTS_OFF_INDICATION_STRING )) == NULL )
    {
      DebugPrintf (1, "\nERROR! NO LIFTS OFF FILENAME FOUND! TERMINATING!");
      Terminate(ERR);
    }
  else
    {
      ship_off_filename = malloc(2000);
      strcpy( ship_off_filename , GRAPHICS_DIR );
      ShipOffPointer += strlen ( LIFTS_OFF_INDICATION_STRING );
      
      ShipOffLength = strstr ( ShipOffPointer , "\n") - ShipOffPointer;
      strncat( ship_off_filename , ShipOffPointer , ShipOffLength );
      ship_off_filename[ strlen(GRAPHICS_DIR) + ShipOffLength ] = 0;
      
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
      strcpy( Crewname , MAP_DIR );
      CrewnamePointer += strlen ( CREWNAME_INDICATION_STRING );
      
      CrewnameLength = strstr (CrewnamePointer , "\n") - CrewnamePointer;
      strncat( Crewname , CrewnamePointer , CrewnameLength );
      Crewname[ strlen(MAP_DIR) + CrewnameLength ] = 0;
      
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
      DebriefingText = malloc ( EndTitleLength +10 );
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
      NextMissionName = malloc ( NextMissionNameLength +10 );
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

  Overall_Average=0.041;
  SkipAFewFrames = 0;

  Init_Video ();

  Init_Audio ();
  
  Init_Joy ();

  Init_Game_Data( MAP_DIR "game.dat" );  // load the default ruleset. This can be overwritten from the mission file.

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

  Draw_Framerate=FALSE;
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
  int ThisTextLength;
#define NEXT_BRIEFING_SUBSECTION_START_STRING "* New Mission Briefing Text Subsection *"
#define END_OF_BRIEFING_SUBSECTION_STRING "* End of Mission Briefing Text Subsection *"

  // STRANGE!! This command will be silently ignored by SDL?
  // WHY?? DONT KNOW!!!
  // Play_Sound ( CLASSICAL_BEEP_BEEP_BACKGROUND_MUSIC );
  // Play_Sound ( CLASSICAL_BEEP_BEEP_BACKGROUND_MUSIC );
  Switch_Background_Music_To ( CLASSICAL_BEEP_BEEP_BACKGROUND_MUSIC );
  // Switch_Background_Music_To ( COMBAT_BACKGROUND_MUSIC_SOUND );

  SDL_SetClipRect ( ne_screen, NULL );
  DisplayImage ( NE_TITLE_PIC_FILE );

  PrepareScaledSurface(TRUE);

  // while (!SpacePressed ());
  // while (SpacePressed());

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
      PreparedBriefingText = malloc (ThisTextLength + 10);
      strncpy ( PreparedBriefingText , NextSubsectionStartPointer , ThisTextLength );
      PreparedBriefingText[ThisTextLength]=0;
      
      // DebugPrintf (1, "\n\nIdentified Text for the scrolling briefing: %s." , PreparedBriefingText);
      fflush(stdout);
      ScrollText ( PreparedBriefingText, SCROLLSTARTX, SCROLLSTARTY, ScrollEndLine);
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

  ScrollText ( DebriefingText , SCROLLSTARTX, SCROLLSTARTY, ScrollEndLine);

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


/* 
----------------------------------------------------------------------
@Desc: This function does the mission debriefing.  If the score was
very good or very bad, player will be asked for this name and the 
highscore list will be updated.

@Ret: 
@Int:
----------------------------------------------------------------------
*/
void
Debriefing (void)
{
  char *tmp_name;
  Hall_entry new, tmp;
  int DebriefColor;
  int count;
  

  return;  // until this works properly


  DebriefColor = FONT_WHITE;
  Me.status = DEBRIEFING;
  SetUserfenster ( DebriefColor );	// KON_BG_COLOR


  count = 1;
  if ( (tmp = highscores) != NULL)
    while (tmp->next) { count++; tmp = tmp->next;}  /* tmp now points to lowest! */
  else
    count = 0;  /* first entry */
 
  if ( (count == MAX_HIGHSCORES) && (RealScore <= tmp->score) )
    return; /* looser! ;) */
      
  /* else: prepare a new entry */
  new = MyMalloc (sizeof(hall_entry));
  new->score = RealScore;
  new->next = new->prev = NULL;
  DisplayText ("Great Score !", User_Rect.x, User_Rect.y, &User_Rect);
  DisplayText ("\nEnter your name: ", User_Rect.x, User_Rect.y, &User_Rect);
  PrepareScaledSurface(TRUE);
  tmp_name = GetString (MAX_NAME_LEN, 2);
  strcpy (new->name, tmp_name);
  free (tmp_name);

  if (!highscores)  /* hey, you're the first one ! */
    highscores = new;
  else   /* link in the new entry */
    {
      count ++;
      tmp = highscores;
      while ( tmp->score >= RealScore )
	tmp = tmp->next;
      ((Hall_entry)(tmp->prev))->next = new;
      new->prev = tmp->prev;
      new->next = tmp;
    }

  /* now check the length of our new highscore list.
   * if longer than MAX_HIGHSCORES */
  tmp = highscores;
  while (tmp->next) tmp = tmp->next; /* find last entry */

  if ( count > MAX_HIGHSCORES ) /* the last one drops out */
    {
      ((Hall_entry)(tmp->prev))->next = NULL;
      free (tmp);
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
  char LoadWhichMission[2000]=MAP_DIR;

  if ( Me.mission.KillOne != (-1) )
    {
      for ( Robot_Counter=0 ; Robot_Counter < MAX_ENEMYS_ON_SHIP ; Robot_Counter++ )
	{
	  if ( ( AllEnemys[Robot_Counter].energy > 0 ) && 
	       ( AllEnemys[Robot_Counter].Status != OUT ) && 
	       ( AllEnemys[Robot_Counter].Marker == Me.mission.KillOne ) )
	    {
	      DebugPrintf (1, "\nOne of the marked droids is still alive...");
	      fflush(stdout);
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
	      // DebugPrintf (1, "\nThere are some robots still alive, and you should kill them all...");
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
	      // DebugPrintf (1, "\nOne of that class is still alive: Nr=%d Lev=%d X=%f Y=%f." , 
	      // Robot_Counter , AllEnemys[Robot_Counter].levelnum , 
	      // AllEnemys[Robot_Counter].pos.x , AllEnemys[Robot_Counter].pos.y );
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

  strcat ( LoadWhichMission , NextMissionName );
  InitNewMission ( LoadWhichMission );
  
} // void CheckIfMissionIsComplete



#undef _init_c



