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
  int Mission_Log_Visible;
  float Mission_Log_Visible_Time;
  float Mission_Log_Visible_Max_Time;
  int Inventory_Visible;
  float Inventory_Visible_Time;
  float Inventory_Visible_Max_Time;
  int CharacterScreen_Visible;
  float CharacterScreen_Visible_Time;
  float CharacterScreen_Visible_Max_Time;
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
  float x;
  float y;
}
moderately_finepoint, *Moderately_finepoint;

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
  char* MissionName;  // this should be the name of the mission, currently uninitialized
  int MissionWasAssigned; // has be influencer been assigned to this mission? , currently uninitialized
  int MissionIsComplete; // has the mission been completed?
  int MissionWasFailed; // has the mission been failed?
  int MissionExistsAtAll; // is this mission entry used at all, or is it just unused memory?
  int AutomaticallyAssignThisMissionAtGameStart; // well...
  // float MissionLastStatusChangeTime; // for how long is the mission now closed?

  int KillAll;
  int KillClass;
  int KillOne;
  int   MustReachLevel;
  point MustReachPoint;
  double MustLiveTime;
  int MustBeClass;
  int MustBeType;
  int MustBeOne;
  
  int ListOfActionsToBeTriggeredAtAssignment[ MAX_MISSION_TRIGGERED_ACTIONS ];
  int ListOfActionsToBeTriggeredAtCompletition[ MAX_MISSION_TRIGGERED_ACTIONS ];
}
mission, *Mission;

// This structure can contain things, that might be triggered by a special
// condition, that can be specified in the mission file as well.
//
typedef struct
{
  char* ActionLabel;  // this is a better reference than a number

  // Maybe the triggered event consists of the influencer saying something
  char* InfluencerSayText;

  // Maybe the triggered event consists of the map beeing changed at some tile
  point ChangeMapLocation;
  int ChangeMapTo;
  int ChangeMapLevel;

  // Maybe the triggered event teleports the influencer somewhere
  point TeleportTarget;
  int TeleportTargetLevel;

  // Maybe the triggered action consists of a mission assignment
  int AssignWhichMission;

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
  // int EventNumber;
  char* TargetActionLabel;
}
event_trigger , *Event_trigger;

typedef struct
{
  int x;
  int y;
  char* Statement_Text;
} map_statement , *Map_statement;

typedef struct
{
  int x;
  int y;
  char* Secret_Code;
} codepanel , *Codepanel;

typedef struct
{
  grob_point inv_size;
  SDL_Surface* Surface;
} item_image_spec , *Item_image_spec;

typedef struct
{
  char* ItemName;
  char* ItemClass;  // class of the item, e.g. weapon, drive, shield, other
  int item_can_be_applied_in_combat;
  int item_can_be_installed_in_influ;
  int item_can_be_installed_in_weapon_slot;
  int item_can_be_installed_in_drive_slot;
  int item_can_be_installed_in_armour_slot;
  int item_can_be_installed_in_shield_slot;
  int item_can_be_installed_in_special_slot;
  int item_can_be_installed_in_aux_slot;
  // double energy_gain_uppon_application_in_combat;
  // double item_weight;

  // How good is the item as drive???
  double item_drive_maxspeed;	// how fast can this item go used as the drive of the droid
  double item_drive_accel;	// as drive, how fast can you accelerate with this item

  // How good is the item as weapon???
  double item_gun_recharging_time;       // time until the next shot can be made, measures in seconds
  int    item_gun_bullet_image_type;       // which type of image to use for displaying this bullet
  int    item_gun_oneshotonly;	        /* if this is set, there is only 1 shot */
  int    item_gun_blast;			/* which blast does this bullet create */
  double item_gun_speed; // how fast should a bullet move straightforward?
  int    base_item_gun_damage; //	damage done by this bullettype 
  int    item_gun_damage_modifier; // modifier to the damage done by this bullettype 
  double item_gun_bullet_lifetime;      // how long does a 'bullet' from this gun type live?
  int    item_gun_bullet_can_be_reflected; // can bullets of this type be reflected by other bullets?
  int    item_gun_bullet_reflect_other_bullets; // can this 'bullet' reflect other bullets
  int    item_gun_bullet_ignore_wall_collisions; // can this bullet pass through walls and map barriers?

  // the following values have only relevance in case of a melee weapon
  double item_gun_angle_change;	// how fast to do a melee weapon swing
  double item_gun_start_angle_modifier;	// where to start with a melee weapon swing
  double item_gun_fixed_offset;         // how far away from the swinger should a melee weapon swing occur?

  // how good is the item as armour or shield or other protection???
  int base_ac_bonus;
  int ac_bonus_modifier;

  // which requirement for strength, dexterity and magic (force) does the item have?
  int item_require_strength;
  int item_require_dexterity;
  int item_require_magic;
  
  // what duration does the item have?
  int base_item_duration;
  int item_duration_modifier;

  // Which picture to use for this item, when it's lying on the floor?
  int picture_number;
  int sound_number;
  int base_list_price;         // the base price of this item at the shop

} itemspec , *Itemspec;



typedef struct
{
  finepoint pos;
  int type;
  int currently_held_in_hand;
  int condition;
  int prefix_code;
  int suffix_code;
  int ac_bonus;    // how much is ac increased by this item worn
  int damage; // how much damage does this item
  int damage_modifier; // how much additional damage can add to the base damage
  int max_duration;     // the maximum item durability reachable for this item
  int gold_amount; // how much cyberbucks are there, IN CASE OF CYBERBUCKS
  float current_duration; // the currently remaining durability for this item
  grob_point inventory_position;
} item, *Item;

typedef struct
{
  char *druidname;
  char *portrait_filename_without_ext;
  int class;
  double maxenergy;		/* the maximum energy the batteries can carry */
  double maxmana;		/* the maximum force this droids mind can carry */
  double lose_health;		/* the energy/time the duid loses under influence-control */
  int aggression;		/* The aggressiveness of this druidtype */
  int flashimmune;		/* is the droid immune to FLASH-bullets */
  int score;			/* score for the elimination of one droid of this type */
  double height;                // the height of this droid  
  double weight;                // the weight of this droid
  int brain;
  int sensor1;
  int sensor2;
  int sensor3;
  // int drive_item;
  item drive_item;
  // int weapon_item;
  item weapon_item;
  // int armour_item;
  item armour_item;
  // int shield_item;
  item shield_item;
  // int special_item;
  item special_item;
  // int aux1_item;
  item aux1_item;
  // int aux2_item;
  item aux2_item;
  int Greeting_Sound_Type; // which sample to play in order to 'greet' the influencer?
  int to_hit; // chance that this droid hits an unarmoured target
  int getting_hit_modifier; // modifier for this droid to receive a hit from the player
  int AdvancedBehaviour;        // Does this droid behave better that in the original paradroid?
  int CallForHelpAfterSufferingHit;  // Does this droid request help from the next console so soon as it is
                                     // hit by a bullet of some type?
  char *notes;			/* notes on the druid of this type */
  int IsHuman;
}
druidspec, *Druidspec;

typedef struct
{
  int type;			/* what kind of druid is this ? */
  int status;			/* attacking, defense, dead, ... */
  finepoint speed;		/* the current speed of the druid */
  finepoint pos;		/* current position in level levelnum */
  double health;		/* the max. possible energy in the moment */
  double energy;		/* current energy level */
  double mana;                  // current mana level 
  double firewait;		// time remaining, until the weapon is ready to fire again...
  double phase;			/* the current phase of animation */
  int autofire;			/* Status of the Firecontrolautomatics */
  int vneut;			/* Status of Velocityneutralizer for the gun */
  int MyFCU;			/* FCU (Fire Control Unit) installed */
  int MyShield;			/* Shield device installed */
  int Shield[4];		/* Status of Partial Shields */
  int Strength;  // character Strength value = 'power supply max. capacity'
  int Magic;     // character Magic value = 
  int Dexterity; // character Dexterity value = 'power redistribution speed'
  int Vitality;  // character Vitality value = 'cloaking field maximum strength'
  long Experience; // character Experience = 'spare droid elements found'
  int PointsToDistribute; // these are the points that are available to distribute upon the character stats
  float Base_Damage; // the current damage the influencer does
  float Damage_Modifier; // the modifier to the damage the influencer currently does
  float AC; // the current Armour Class of the influencer
  // float RechargeTimeModifier; // the factor to be multiplied to the recharge time (times 100 for % notation)
  // float RechargeTime; // the actual minimum time to wait for the influencer between shots
  float to_hit;
  int exp_level; // which 'experience level' is the influencer currenly at?
  long ExpRequired; // how much experience required for the next level?
  long Gold;
  char character_name[25];
  gps Position_History_Ring_Buffer[ MAX_INFLU_POSITION_HISTORY ];
  mission AllMissions[ MAX_MISSIONS_IN_GAME ];         // What must be done to fullfill this mission?
  float MissionTimeElapsed;
  int Marker;                   // In case you've taken over a marked droid, this will contain the marker
  float LastCrysoundTime;
  float LastTransferSoundTime;
  float TextVisibleTime;
  char* TextToBeDisplayed;
  float Current_Victim_Resistance_Factor;
  int FramesOnThisLevel;        // how many frames has the influ spent on this level already?
  item Inventory[ MAX_ITEMS_IN_INVENTORY ];
  int KillRecord[ 1000 ];      // how many ( of the first 1000 monster types) have been killed yet?
}
influence_t, *Influence_t;

typedef struct
{
  char* RequestTrigger; // which word of the influencer initiates the request
  char* RequestText;  // which text will be used to pose the request
  int RequestRequiresMissionDone; // which mission must be done in order to get this request
  int RequestRequiresMissionUnassigned; // which mission must be unassigned in order to get this request
  char* AnswerYes;    // what does the influ have to say to confirm?
  char* AnswerNo;     // what does the influ have to say to reject?
  char* ResponseYes;  // what will the droid respond, if influ confirms the question
  char* ResponseNo;   // what will the droid respond, if influ denies the question
  char* ActionTrigger; // label of the action, that is to be set of on confirmation of the request
} request, *Request;

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
  // point PrivatePathway[ MAX_STEPS_IN_GIVEN_COURSE ];
  moderately_finepoint PrivatePathway[ MAX_STEPS_IN_GIVEN_COURSE ];
  float TextVisibleTime;
  char* TextToBeDisplayed;
  int has_greeted_influencer;
  int NumberOfPeriodicSpecialStatements;
  char **PeriodicSpecialStatements;
  char* QuestionResponseList[ MAX_CHAT_KEYWORDS_PER_DROID * 2 ];  // even indices for keywords, odd for answers 

  request RequestList[ MAX_REQUESTS_PER_DROID ];
}
enemy, *Enemy;

typedef struct
{
  // double recharging_time;       // time until the next shot can be made, measures in seconds
  int phases;			/* how many phases in motion to show */
  double phase_changes_per_second; // how many different phases to display every second
  SDL_Surface *SurfacePointer[ MAX_PHASES_IN_A_BULLET ];   // A pointer to the surfaces containing 
                                                           // the bullet images of this bullet
} 
bulletspec, *Bulletspec;

typedef struct
{
  finepoint pos;
  finepoint speed;
  int type;
  byte phase;
  int damage; // damage done by this particular bullet 
  int time_in_frames;    // how long does the bullet exist, measured in number of frames
  double time_in_seconds; // how long does the bullet exist in seconds
  double bullet_lifetime; // how long can this bullet exist at most
  signed char mine;
  int owner;
  double angle;
  byte total_miss_hit [ MAX_ENEMYS_ON_SHIP ] ;
  byte miss_hit_influencer;
  int to_hit;
  int ignore_wall_collisions;
  int was_reflected;

  // these are values only of relevance in case of a melee weapon
  double angle_change_rate;
  float fixed_offset;
  finepoint* owner_pos;

  // these are technical parameters, not for the game behaviour
  SDL_Surface *SurfacePointer[ MAX_PHASES_IN_A_BULLET ];
  int Surfaces_were_generated; // 
}
bullet, *Bullet;

typedef struct
{
  int phases;
  unsigned char *picpointer;
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
  map_statement StatementList[ MAX_STATEMENTS_PER_LEVEL ];
  codepanel CodepanelList[ MAX_CODEPANELS_PER_LEVEL ];
  int xlen;		/* X dimension */
  int ylen;
  int color;
  char *map[MAXMAPLINES];	/* this is a vector of pointers ! */
  grob_point refreshes[MAX_REFRESHES_ON_LEVEL];
  grob_point teleporters[MAX_TELEPORTERS_ON_LEVEL];
  grob_point doors[MAX_DOORS_ON_LEVEL];
  waypoint AllWaypoints[MAXWAYPOINTS];
  item ItemList[ MAX_ITEMS_PER_LEVEL ];
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
