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
  char signature;
  char version;
  char encoding;
  char bytes_per_pixel;
  unsigned short int xmin;
  unsigned short int ymin;
  unsigned short int xmax;
  unsigned short int ymax;
  unsigned short int vres;
  unsigned short int hres;
  char palette[48];
  char reserved;
  char color_layers;
  unsigned short int bytes_per_line;
  unsigned short int palette_type;
  char unused[58];
}
PCX_Header;

typedef struct
{
  long PlayerScore;
  void *NextPlayer;
  char *PlayerName;
}
HallElement;

typedef struct
{
  unsigned char rot;
  unsigned char gruen;
  unsigned char blau;
}
color, *Color;

typedef struct
{
  signed short x;
  signed short y;
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
  const char *druidname;
  double maxspeed;		/* the maximum of speed it can go */
  int class;
  double accel;			/* its acceleration */
  double maxenergy;		/* the maximum energy the batteries can carry */
  double lose_health;		/* the energy/time the duid loses under influence-control */
  byte gun;			/* Which gun does this druid use */
  byte vneutral;		/* Is there a velocityneutralisator for Bullets ? */
  byte aggression;		/* The aggressiveness of this druidtype */
  byte firewait;		/* Wait approx. after shoot, in 1/4 sec. */
  byte flashimmune;		/* is the droid immune to FLASH-bullets */
  int score;			/* Punkte f"ur die Vernichtung dieses Modells */
  char *notes;			/* notes on the druid of this type */
  // char *image;
}
druidspec, *Druidspec;

typedef struct
{
  int type;			/* what kind of druid is this ? */
  byte status;			/* attacking, defense, dead, ... */
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
}
influence_t, *Influence_t;

typedef struct
{
  byte type;			/* gibt die Nummer in Druidmap an */
  int levelnum;			/* Level in dem sich enemy befindet */
  finepoint pos;		/* gibt die Koordinaten der Momentanposition an */
  finepoint speed;		/* current speed  */
  double energy;			/* gibt die Energie dieses Robots an */
  double feindphase;		/* gibt die Phase an in der der Feind gedreht ist */
  byte nextwaypoint;		/* gibt den naechsten Zielpunkt an */
  byte lastwaypoint;		/* Waypoint, von dem ausgegangen wurde */
  byte Status;			/* gibt z.B. an ob der Robotter abgeschossen wurde */
  double warten;			/* gibt Wartezeit an bis Fahrt wieder aufgenommen wird */
  byte passable;		/* Zeit (counter), in der druid passable ist */
  double firewait;		/* gibt die Zeit bis zum naechsten Schuss an */
  // byte onscreen;		/* gibt an ob der Robot im moment sichtbar ist */
}
enemy, *Enemy;

typedef struct
{
  double speed;			/* speed of the bullet */
  int damage;			/* damage done by this bullettype */
  int time;		        // ??? SENSELESS VARIABLE?  how long does bullet exist, measured in number of frames
  byte phases;			/* how many phases in motion to show */
  byte blast;			/* which blast does this bullet create */
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

  /* connections: Numbers in Elevator-Array */
  signed char up;
  signed char down;

  /* row */
  byte elevator_row;		/* number of elev. column */
}
elevator, *Elevator;

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
  unsigned int xlen;		/* X dimension */
  unsigned int ylen;
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
  elevator AllElevators[MAX_LIFTS];
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
