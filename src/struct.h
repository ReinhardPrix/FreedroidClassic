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

#ifndef _struct_h
#define _struct_h

#include "defs.h"

#ifndef _BYTE
typedef unsigned char byte;
#define _BYTE
#endif
#ifndef _WORD
typedef unsigned word;
#define _WORD
#endif
#ifndef _BOOL
typedef char bool;
#define _BOOL
#endif

typedef struct
{
  char name[MAX_NAME_LEN+5];
  long score;                  /* use -1 for an empty entry */
  char date[DATE_LEN+5];
  int mission;    /* classic mission or extensions? */
}
highscore_entry, *Highscore_entry;

typedef struct
{
  unsigned char rot;
  unsigned char gruen;
  unsigned char blau;
}
color, *Color;

typedef struct
{
  float WantedTextVisibleTime;
  int Draw_Framerate;
  int Draw_Energy;
  int Draw_Position;
  int Influencer_Refresh_Text;
  int Influencer_Blast_Text;
  int Enemy_Hit_Text;
  int Enemy_Bump_Text;
  int Enemy_Aim_Text;
  int All_Texts_Switch;
  float Current_BG_Music_Volume;
  float Current_Sound_FX_Volume;
  float Current_Gamma_Correction;
  int StandardEnemyMessages_On_Off;
  int StandardInfluencerMessages_On_Off;
  int Mouse_Input_Permitted;
  char* Theme_SubPath;
  int FullUserRect;   // use "full" or "classic" (=small) User_Rect
}
configuration_for_freedroid , *Configuration_for_freedroid;



typedef struct
{
  int x;
  int y;
}
point, *Point;

typedef struct
{
  double x;
  double y;
}
finepoint, *Finepoint;

typedef struct
{
  double x;
  double y;
  int z;
} gps, *GPS;

typedef struct
{
  signed char x;
  signed char y;
}
grob_point, *grob_Point;


typedef grob_point vect;
typedef grob_Point Vect;

typedef struct
{
  int KillAll;
  int KillClass;
  int KillOne;
  int   MustReachLevel;
  point MustReachPoint;
  double MustLiveTime;
  int MustBeClass;
  int MustBeType;
  int MustBeOne;
}
missiontarget, *Missiontarget;

// This structure can contain things, that might be triggered by a special
// condition, that can be specified in the mission file as well.
//
typedef struct
{
  // Maybe the triggered event consists of the influencer saying something
  int InfluencerSaySomething;
  char* InfluencerSayText;
  // Maybe the triggered event consists of the map beeing changed at some tile
  int ChangeMap;
  point ChangeMapLocation;
  int ChangeMapTo;
  int ChangeMapLevel;

  // Maybe the triggered event consists of ??????

}
triggered_action , *Triggered_action;

// This structure can contain conditions that must be fulfilled, so that a special
// event is triggered.  Such conditions may be specified in the mission file as well
//
typedef struct
{
  // Maybe the event is triggerd by the influencer stepping somewhere
  int Influ_Must_Be_At_Level;
  point Influ_Must_Be_At_Point;

  // Maybe the event is triggered by time
  float Mission_Time_Must_Have_Passed;
  float Mission_Time_Must_Not_Have_Passed;

  int DeleteTriggerAfterExecution;
  // And now of course which event to trigger!!!!
  // Thats propably the most important information at all!!!
  int EventNumber;
}
event_trigger , *Event_trigger;



typedef struct
{
  char *druidname;
  double maxspeed;		/* the maximum of speed it can go */
  int class;
  double accel;			/* its acceleration */
  double maxenergy;		/* the maximum energy the batteries can carry */
  double lose_health;		/* the energy/time the duid loses under influence-control */
  int gun;			/* Which gun does this druid use */
  int aggression;		/* The aggressiveness of this druidtype */
  int flashimmune;		/* is the droid immune to FLASH-bullets */
  int score;			/* score for the elimination of one droid of this type */
  double height;                // the height of this droid  
  double weight;                // the weight of this droid
  int drive;
  int brain;
  int sensor1;
  int sensor2;
  int sensor3;
  int armament;
  int AdvancedBehaviour;        // Does this droid behave better that in the original paradroid?
  int CallForHelpAfterSufferingHit;  // Does this droid request help from the next console so soon as it is
                                     // hit by a bullet of some type?
  char *notes;			/* notes on the druid of this type */
}
druidspec, *Druidspec;

typedef struct
{
  int type;			/* what kind of druid is this ? */
  int status;			/* attacking, defense, dead, ... */
  finepoint speed;		/* the current speed of the druid */
  finepoint pos;		/* current position in level levelnum */
  double health;		/* the max. possible energy in the moment */
  double energy;		/* current energy */
  double firewait;		/* counter after fire */
  double phase;			/* the current phase of animation */
  int autofire;			/* Status of the Firecontrolautomatics */
  int vneut;			/* Status of Velocityneutralizer for the gun */
  int MyFCU;			/* FCU (Fire Control Unit) installed */
  int MyShield;			/* Shield device installed */
  int Shield[4];		/* Status of Partial Shields */
  gps Position_History_Ring_Buffer[ MAX_INFLU_POSITION_HISTORY ];
  // finepoint Position_History[ MAX_INFLU_POSITION_HISTORY ]; // History of the places the influ has been during the last 10 frames
  missiontarget mission;         // What must be done to fullfill this mission?
  float MissionTimeElapsed;
  int Marker;                   // In case you've taken over a marked droid, this will contain the marker
  float LastCrysoundTime;
  float LastTransferSoundTime;
  float TextVisibleTime;
  char* TextToBeDisplayed;
  float Current_Victim_Resistance_Factor;
  int FramesOnThisLevel;        // how many frames has the influ spent on this level already?
}
influence_t, *Influence_t;

typedef struct
{
  int type;			/* gibt die Nummer in Druidmap an */
  int levelnum;			/* Level in dem sich enemy befindet */
  finepoint pos;		/* gibt die Koordinaten der Momentanposition an */
  finepoint speed;		/* current speed  */
  double energy;		/* gibt die Energie dieses Robots an */
  double feindphase;		/* gibt die Phase an in der der Feind gedreht ist */
  int nextwaypoint;		/* gibt den naechsten Zielpunkt an */
  int lastwaypoint;		/* Waypoint, von dem ausgegangen wurde */
  int Status;			/* gibt z.B. an ob der Robotter abgeschossen wurde */
  double warten;		// time till the droid will start to move again
  byte passable;		/* Zeit (counter), in der druid passable ist */
  double firewait;		/* gibt die Zeit bis zum naechsten Schuss an */
  // byte onscreen;		/* gibt an ob der Robot im moment sichtbar ist */
  int CompletelyFixed;          // set this flat to make the robot entirely immobile
  int FollowingInflusTail;      // does this robot follow influs tail? (trott behind him? )
  int SpecialForce;             // This flag will exclude the droid from initial shuffling of droids
  int Marker;                   // This provides a marker for special mission targets
  int AdvancedCommand;          // An advanced command that modifies the behaviour of the droid (in new missions)
  double Parameter1;            // This contains special information for AdvancedCommand
  double Parameter2;            // This contains special information for AdvancedCommand
  int Friendly;                 // is this a friendly droid or is it a MS controlled one?
  int persuing_given_course;    // is this robot persuing a given course via PersueGivenCourse( EnemyNum )?
  int StayHowManyFramesBehind;  // how many frames shall this droid trott behind the influ when follwing his tail?
  int StayHowManySecondsBehind;  // how many seconds shall this droid trott behind the influ when follwing his tail?
  point PrivatePathway[ MAX_STEPS_IN_GIVEN_COURSE ];
  float TextVisibleTime;
  char* TextToBeDisplayed;
  int NumberOfPeriodicSpecialStatements;
  char **PeriodicSpecialStatements;
  char* QuestionResponseList[ MAX_CHAT_KEYWORDS_PER_DROID * 2 ];  // even indices for keywords, odd for answers 
}
enemy, *Enemy;

typedef struct
{
  double recharging_time;       // time until the next shot can be made, measures in seconds
  double speed;			/* speed of the bullet */
  int damage;			/* damage done by this bullettype */
  int phases;			/* how many phases in motion to show */
  double phase_changes_per_second; // how many different phases to display every second
  int blast;			/* which blast does this bullet create */
  int oneshotonly;	        /* if this is set, there is only 1 shot */
  SDL_Surface *SurfacePointer[ MAX_PHASES_IN_A_BULLET ];   // A pointer to the surfaces containing 
                                                           // the bullet images of this bullet
  // SDL_Rect *block;            /* the coordinates of the blocks in ne_blocks */
}
bulletspec, *Bulletspec;

typedef struct
{
  finepoint pos;
  finepoint prev_pos;	 // use this for improved collision checks (for low FPS machines)
  finepoint speed;
  byte type;
  byte phase;
  int time_in_frames;    // how long does the bullet exist, measured in number of frames
  double time_in_seconds; // how long does the bullet exist in seconds
  signed char mine;
  int owner;
  double angle;
  SDL_Surface *SurfacePointer[ MAX_PHASES_IN_A_BULLET ];
  int Surfaces_were_generated; // 
}
bullet, *Bullet;

typedef struct
{
  int phases;
  unsigned char *picpointer;
  SDL_Rect *block;     /* the coordinates of the blocks in ne_blocks */
  double total_animation_time;
  SDL_Surface *SurfacePointer[ MAX_PHASES_IN_A_BULLET ];   // A pointer to the surfaces containing 
                                                           // the blast images of this blast type
}
blastspec, *Blastspec;

typedef struct
{
  double PX;			/* PosX */
  double PY;			/* PosY */
  int type;
  double phase;
  int MessageWasDone;
  
}
blast, *Blast;

typedef struct
{
  byte level;			/* Level - 'Koordinate' */
  int x;			/* x,y Koordinaten */
  int y;
}
location, *Location;

typedef struct
{
  int level;   // The level, where this elevtor entrance is located
  int x;       // The position in x of this elevator entrance within the level
  int y;       // The position in y of this elevator entrance within the level

  /* connections: Numbers in Lift-Array */
  int up;
  int down;

  int lift_row;  // which lift column does this lift entrance belong to?
}
lift, *Lift;

typedef struct
{
  byte x;			/* Grob */
  byte y;
  signed char connections[MAX_WP_CONNECTIONS];
}
waypoint, *Waypoint;

typedef struct
{
  int empty;
  int levelnum;	/* Number of this level */
  char *Levelname;		/* Name of this level */
  char *Background_Song_Name;
  char *Level_Enter_Comment;
  int xlen;		/* X dimension */
  int ylen;
  int color;
  char *map[MAXMAPLINES];	/* this is a vector of pointers ! */
  grob_point refreshes[MAX_REFRESHES_ON_LEVEL];
  grob_point teleporters[MAX_TELEPORTERS_ON_LEVEL];
  grob_point doors[MAX_DOORS_ON_LEVEL];
  waypoint AllWaypoints[MAXWAYPOINTS];
}
level, *Level;

typedef struct
{
  int num_levels;
  int num_lifts;
  int num_lift_rows;
  char* AreaName;
  Level AllLevels[MAX_LEVELS];
  lift  AllLifts[MAX_LIFTS];
  SDL_Rect LiftRow_Rect[MAX_LIFT_ROWS];   /* the lift-row rectangles */
  SDL_Rect Level_Rects[MAX_LEVELS][MAX_LEVEL_RECTS];  /* level rectangles */
  int num_level_rects[MAX_LEVELS];  /* how many rects has a level */
}
ship, *Ship;


typedef struct
{
  char *FCUName;
}
FCU;


typedef struct
{
  point pos;
  int len;
  int hgt;
  int oldval;
  int col;
}
bar, *Bar;

#endif
