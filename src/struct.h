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
  void *next;    /* a list is easier to handle than an array !*/
  void *prev;
}
hall_entry, *Hall_entry;

typedef struct
{
  unsigned char rot;
  unsigned char gruen;
  unsigned char blau;
}
color, *Color;

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
  char *notes;			/* notes on the druid of this type */
}
druidspec, *Druidspec;

typedef struct
{
  int type;			/* what kind of druid is this ? */
  int status;			/* attacking, defense, dead, ... */
  finepoint speed;		/* the current speed of the druid */
  finepoint pos;		/* current position in level levelnum */
  double health;			/* the max. possible energy in the moment */
  double energy;			/* current energy */
  double firewait;		/* counter after fire */
  double phase;			/* the current phase of animation */
  int autofire;			/* Status of the Firecontrolautomatics */
  int vneut;			/* Status of Velocityneutralizer for the gun */
  int MyFCU;			/* FCU (Fire Control Unit) installed */
  int MyShield;			/* Shield device installed */
  int Shield[4];		/* Status of Partial Shields */
  finepoint Position_History[11]; // History of the places the influ has been during the last 10 frames
  missiontarget mission;         // What must be done to fullfill this mission?
  float MissionTimeElapsed;
  int Marker;                   // In case you've taken over a marked droid, this will contain the marker
  float LastCrysoundTime;
  float LastTransferSoundTime;
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
  double warten;		/* gibt Wartezeit an bis Fahrt wieder aufgenommen wird */
  byte passable;		/* Zeit (counter), in der druid passable ist */
  double firewait;		/* gibt die Zeit bis zum naechsten Schuss an */
  // byte onscreen;		/* gibt an ob der Robot im moment sichtbar ist */
  int CompletelyFixed;          // set this flat to make the robot entirely immobile
  int SpecialForce;             // This flag will exclude the droid from initial shuffling of droids
  int Marker;                   // This provides a marker for special mission targets
  int AdvancedCommand;          // An advanced command that modifies the behaviour of the droid (in new missions)
  double Parameter1;            // This contains special information for AdvancedCommand
  double Parameter2;            // This contains special information for AdvancedCommand
  int Friendly;                 // is this a friendly droid or is it a MS controlled one?
}
enemy, *Enemy;

typedef struct
{
  double recharging_time;       // time until the next shot can be made, measures in seconds
  double speed;			/* speed of the bullet */
  int damage;			/* damage done by this bullettype */
  int phases;			/* how many phases in motion to show */
  int blast;			/* which blast does this bullet create */
  unsigned char *picpointer;	/* pointer to picture of bullet */
  signed char oneshotonly;	/* if this is set, there is only 1 shot */
  int WaitNextTime;
  SDL_Rect *block;            /* the coordinates of the blocks in ne_blocks */
}
bulletspec, *Bulletspec;

typedef struct
{
  finepoint pos;
  finepoint speed;
  byte type;
  byte phase;
  int time_in_frames;           // how long does the bullet exist, measured in number of frames
  double time_in_seconds;        // how long does the bullet exist in seconds
  signed char mine;
  int owner;
}
bullet, *Bullet;

typedef struct
{
  byte phases;
  unsigned char *picpointer;
  SDL_Rect *block;     /* the coordinates of the blocks in ne_blocks */
}
blastspec, *Blastspec;

typedef struct
{
  double PX;			/* PosX */
  double PY;			/* PosY */
  byte type;
  double phase;
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
  /* current location */
  byte level;
  byte x;			/* Grob */
  byte y;

  /* connections: Numbers in Lift-Array */
  signed char up;
  signed char down;

  /* row */
  byte lift_row;
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
  byte empty;
  unsigned int levelnum;	/* Number of this level */
  char *Levelname;		/* Name of this level */
  int xlen;		/* X dimension */
  int ylen;
  unsigned int color;
  char *map[MAXMAPLINES];	/* this is a vector of pointers ! */
  grob_point refreshes[MAX_REFRESHES_ON_LEVEL];
  grob_point doors[MAX_DOORS_ON_LEVEL];
  waypoint AllWaypoints[MAXWAYPOINTS];
}
level, *Level;

typedef struct
{
  int num_levels;
  int num_lifts;
  int num_lift_rows;
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
