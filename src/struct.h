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

typedef struct {
	long PlayerScore;
	void* NextPlayer;
	char* PlayerName;
} HallElement;

typedef struct {
	unsigned char rot;
	unsigned char gruen;
	unsigned char blau;
} color, *Color;

typedef struct {
   signed short x;
   signed short y;
} point, *Point;

typedef struct {
   signed char x;
   signed char y;
} grob_point, *grob_Point;


typedef grob_point vect;
typedef grob_Point Vect;


typedef struct {
   const char *druidname;
   byte maxspeed;    /* the maximum of speed it can go */
   char class;
   byte accel;       /* its acceleration */
   int maxenergy;    /* the maximum energy the batteries can carry */
   byte lose_health; /* the energy/time the duid loses under influence-control */
   byte gun;         /* Which gun does this druid use */
   byte vneutral;		/* Is there a velocityneutralisator for Bullets ? */
	byte aggression;	/* The aggressiveness of this druidtype */
	byte firewait;		/* Wait approx. after shoot, in 1/4 sec. */
	byte flashimmune;	/* is the droid immune to FLASH-bullets */
	int score;			/* Punkte f"ur die Vernichtung dieses Modells */
   char *notes;		/* notes on the druid of this type */
} druidspec, *Druidspec;

typedef struct {
   byte type;         /* what kind of druid is this ? */
   byte status;       /* attacking, defense, dead, ... */
   vect speed;       /* the current speed of the druid */
   point pos;        /* current position in level levelnum */
   int health;			/* the max. possible energy in the moment */
   int energy;       /* current energy */
	byte firewait;		/* counter after fire */
   byte  phase;        /* the current phase of animation */
   int autofire;		/* Status of the Firecontrolautomatics */
   int vneut;			/* Status of Velocityneutralizer for the gun */
   int MyFCU;			/* FCU (Fire Control Unit) installed */
   int MyShield;		/* Shield device installed */
   int Shield[4];		/* Status of Partial Shields */
} influence_t, *Influence_t;

typedef struct {
	byte type;			/* gibt die Nummer in Druidmap an */
	byte levelnum;		/* Level in dem sich enemy befindet */
	point pos;			/* gibt die Koordinaten der Momentanposition an */
	vect speed;			/* current speed  */
	int energy;		/* gibt die Energie dieses Robots an */
	byte feindphase;	/* gibt die Phase an in der der Feind gedreht ist */
	int feindrehcode;	/* gibt Drehungen von Bruchteilen von Dehphasen an */
	byte nextwaypoint;	/* gibt den naechsten Zielpunkt an */
	byte lastwaypoint;	/* Waypoint, von dem ausgegangen wurde */
	byte Status;			/* gibt z.B. an ob der Robotter abgeschossen wurde */
	byte warten;			/* gibt Wartezeit an bis Fahrt wieder aufgenommen wird */
	byte passable;			/* Zeit (counter), in der druid passable ist */
	byte firewait;		/* gibt die Zeit bis zum naechsten Schuss an */
	byte onscreen;		/* gibt an ob der Robot im moment sichtbar ist */
	int Shield[4];		// Status of Partial shields
} enemy, *Enemy;

typedef struct {
   byte speed;        /* speed of the bullet */
   byte damage;					/* damage done by this bullettype */
   byte time;              /* how long does bullet exist */
   byte phases;             /* how many phases in motion to show */
   byte blast;              /* which blast does this bullet create */
   unsigned char *picpointer; /* pointer to picture of bullet */
   signed char oneshotonly;	/* if this is set, there is never more than 1 shot from this Influweapon */
   int WaitNextTime;
} bulletspec, *Bulletspec;

typedef struct {
   int PX;
   int PY;
   signed char SX; /* SpeedX */
   signed char SY; /* SpeedY */
   byte type;
   byte phase;
   byte time;
   signed char mine;
   int owner;
} bullet, *Bullet;

typedef struct {
   byte phases;
   unsigned char *picpointer;
} blastspec, *Blastspec;

typedef struct {
   int PX;  /* PosX */
   int PY;  /* PosY */
   byte type;
   byte phase;
} blast, *Blast;

typedef struct {
	byte level;	/* Level - 'Koordinate' */
	int x;	/* x,y Koordinaten */
	int y;
} location, *Location;

typedef struct {
	/* current location */
	byte level;
	byte x;		/* Grob */
	byte y;
	
	/* connections: Numbers in Elevator-Array */
	signed char up;
	signed char down;

	/* row */
	byte elevator_row; /* number of elev. column */
} elevator, *Elevator;

typedef struct {
	byte x;		/* Grob */
	byte y;
	signed char connections[MAX_WP_CONNECTIONS];
} waypoint, *Waypoint;

typedef struct {
	byte empty;
   byte levelnum;				/* Number of this level */
   char *Levelname;			/* Name of this level */
   byte xlen;    /* X dimension */
   byte ylen;
   byte color;
   char *map[MAXMAPLINES]; /* this is a vector of pointers ! */
   grob_point refreshes[MAX_REFRESHES_ON_LEVEL];
   grob_point doors[MAX_DOORS_ON_LEVEL];
   waypoint AllWaypoints[MAXWAYPOINTS];
} level, *Level; 

typedef struct {
	int LevelsOnShip;
	Level AllLevels[MAX_LEVELS_ON_SHIP];
	elevator AllElevators[ALLELEVATORS];
} ship, *Ship;


typedef struct {
	char* FCUName;
} FCU;

typedef struct {
	char* shieldname;
} shieldspec, *Shieldspec;

typedef struct {
	point pos;
	int len;
	int hgt;
	int oldval;
	int col;
} bar, *Bar;

#endif

/*=@Header==============================================================
 * $Source$
 *
 * @Desc: struct and typedef definitions 
 *    
 * $Revision$
 * $State$
 *
 * $Author$
 *
 * $Log$
 * Revision 1.9  2002/04/08 09:53:13  rp
 * Johannes' initial linux PORT
 *
 * Revision 1.7  1994/06/19  16:41:38  prix
 * Thu Sep 30 13:57:49 1993: Header moved to end of file
 * Sun Oct 03 09:07:38 1993: enemy-struct hat "onscreen" dazubekommen
 * Sun Oct 03 09:16:34 1993: "flashimmune" added to druidspec
 * Sun Oct 03 09:23:53 1993: "time" added to bullet
 * Mon Oct 04 15:30:32 1993: reduced sizes of structs wherever possible
 * Sun Oct 17 09:02:30 1993: vneutral added to druidspec
 * Mon Oct 25 14:23:04 1993: enemy-struct um (byte passable) erweitert
 * Tue Jun 14 10:47:50 1994: added structure for bar
 * Wed Jun 15 20:06:11 1994: Shield[4] added to enemy
 *
 * Revision 1.6  1993/09/30  17:57:20  prix
 * Fri Aug 06 14:56:25 1993: added first version of ship-struct
 * Fri Aug 06 15:29:06 1993: removed levelname from level-struct
 * Sat Aug 07 15:53:45 1993: made location more "plain"
 * Sat Aug 07 16:03:52 1993: changed elevator struct
 * Sun Aug 08 17:58:49 1993: little change in  ship-struct
 * Sun Aug 08 20:53:11 1993: added struct waypoint and changed enemy - struct
 * Mon Aug 09 20:00:15 1993: added emty to level-struct
 * Mon Aug 09 20:01:37 1993: added color to level-struct
 * Mon Aug 09 21:10:50 1993: changes in bulletspec: time and in bullet: timecounter
 * Tue Aug 10 14:52:11 1993: removed timecounter from bulletstruct
 * Sat Aug 14 09:38:26 1993: firewait now in druidspec
 * Sat Aug 21 19:50:22 1993: added refreshes array to level-struct
 * Tue Aug 24 17:29:30 1993: lose_health and health added
 * Thu Sep 30 13:56:59 1993: No picpointer in Druidspec !!
 *
 * Revision 1.5  1993/08/03  18:31:23  prix
 * Mon May 24 20:18:48 1993: *picture in struct druidmodel to type void*
 * Mon May 24 20:20:54 1993: druidname is now const char*
 * Tue May 25 14:35:29 1993: Bullet auf Absolutkoordinaten reduziert
 * Fri May 28 17:36:51 1993: added struct guntype
 * Fri May 28 19:47:59 1993: made picpointers far
 * Fri May 28 21:20:44 1993: added phase-vars to druid-structs
 * Fri May 28 21:46:09 1993: changed Blast-struct: no grob/fein pos !
 * Fri May 28 22:05:32 1993: made point and vect structs with signed ints
 * Sat May 29 22:38:29 1993: made map-array char in struct Level
 * Sun May 30 10:16:26 1993: map in Levelstruct ist wieder int !
 * Sun May 30 10:45:58 1993: map is char* again
 * Sun May 30 18:50:46 1993: renamed a bit
 * Sun May 30 19:27:16 1993: additions to druidspec struct
 * Sun May 30 20:36:27 1993: added blastspec struct
 * Mon May 31 14:40:55 1993: made blast/bullet the struct and Blast/Bullet a pointer to it
 * Tue Jun 01 08:49:41 1993: druidspec includes maxenergy now
 * Tue Jun 01 11:57:32 1993: bulletspec includes the blast it makes now
 * Tue Jun 01 16:08:09 1993: detabbed for printing
 * Sat Jul 24 14:15:17 1993: enemy-Struktur eingefuehrt
 * Sat Jul 24 16:25:28 1993: feindphase eingefuehrt
 * Sun Jul 25 07:53:45 1993: aenderung der Struct enemy: waypoints und point-Koordinaten
 * Mon Jul 26 12:33:49 1993: notes on the druid in the structure
 * Thu Jul 29 07:39:06 1993: warten in enemy-struct eingefuehrt
 * Thu Jul 29 09:23:23 1993: PX,PY,SX,SY sind int
 * Thu Jul 29 12:14:28 1993: Elevator struct eingefuehrt ???
 * Thu Jul 29 12:34:29 1993: energy auf damage umbenannt
 * Sat Jul 31 07:41:52 1993: struct color hinzugefuegt
 * Sat Jul 31 10:46:56 1993: waypoints - array now out of enemy-array (easier to init !)
 * Sat Jul 31 11:35:08 1993: added firewait to druidspec
 * Sat Jul 31 11:49:35 1993: changed druidspec: no phases, turnable; added aggression
 * Sat Jul 31 12:05:02 1993: added levelnum to level-struct
 * Sat Jul 31 12:11:05 1993: removed firewait again: in bulletspec-struct !!
 * Sat Jul 31 12:15:12 1993: changed druid-struct: firewait instead of firedir
 * Sat Jul 31 12:22:59 1993: changed nofire in enemy to firewait
 * Sat Jul 31 12:50:44 1993: Influence now has his own type
 * Sat Jul 31 17:47:35 1993: firedir added to Influence_t -struct
 * Sat Jul 31 19:43:57 1993: added levelnum to enemy struct
 * Sat Jul 31 21:30:52 1993: added doorsarray to level struct
 * Tue Aug 03 14:27:31 1993: removed drive from druidspec-struct
 * Tue Aug 03 14:31:16 1993: removed firedir and levelnum of influence_t struct
 *
 * Revision 1.4  1993/05/23  21:07:55  prix
 * Sun May 23 16:15:04 1993: added druid and druidmodel structs: first concepts
 *
 * Revision 1.3  1993/05/23  19:33:53  prix
 * Sat May 22 18:21:16 1993: Soundblastertypen hizugefuegt
 * Sat May 22 18:52:08 1993: Zuruecknahmen
 *
 * Revision 1.2  1993/05/22  21:58:42  rp
 * added level struct
 *
 * Revision 1.1  1993/05/22  20:56:16  rp
 * Initial revision
 *
 *
 *-@Header------------------------------------------------------------*/

