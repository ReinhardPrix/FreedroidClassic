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

/* 
 *  _Definitions_ of global variables
 * This file should only be included in main.c, and 
 * the variable _declarations_ should be made in global.h under _main_c
 *
 */


#include "colodefs.h"
#include "ship.h"

SDL_Rect Full_Screen_Rect = { 0, 0, SCREENLEN, SCREENHEIGHT };
SDL_Rect Classic_User_Rect = {
  (SCREENLEN-9*INITIAL_BLOCK_WIDTH) / 2, 2*(SCREENHEIGHT-4*INITIAL_BLOCK_HEIGHT)/3,
  9*INITIAL_BLOCK_WIDTH,  4*INITIAL_BLOCK_HEIGHT
};

// SDL_Rect Full_User_Rect = {0, BANNER_HEIGHT+10, SCREENLEN, SCREENHEIGHT-BANNER_HEIGHT-11};
SDL_Rect Full_User_Rect = {0, BANNER_HEIGHT, SCREENLEN, SCREENHEIGHT-BANNER_HEIGHT-1};

SDL_Rect Cons_Rect = {16, 162, 595, 315};

SDL_Rect Cons_Menu_Rect = {32, 180, CONS_MENU_LENGTH, CONS_MENU_HEIGHT};
SDL_Rect Cons_Text_Rect = {175, 180, SCREENLEN-175, 305}; 


int Shieldcolors[INITIAL_BLOCK_WIDTH];

int ShipEmptyCounter = 0;	/* counter to Message: you have won(this ship */

influence_t Me = {
  DRUID001, WAR_BOT, TRANSFERMODE, {0, 0}, {120, 48}, 100, 100, 0, 0
};

/*
enum _status
{
  MOBILE,
  TRANSFERMODE,
  WEAPON,
  CAPTURED,
  COMPLETE,
  REJECTED,
  CONSOLE,
  DEBRIEFING,
  TERMINATED,
  PAUSE,
  CHEESE,
  ELEVATOR,
  BRIEFING
};
*/

char *InfluenceModeNames[] = {
  "Mobile",
  "Transfer",
  "Weapon",
  "Captured",
  "Complete",
  "Rejected",
  "Logged In",
  "Debriefing",
  "Terminated",
  "Pause",
  "Cheese",
  "Elevator",
  "Briefing",
  "Menu",
  NULL
};


char *Classname[] = {
  "Influence device",
  "Disposal robot",
  "Servant robot",
  "Messenger robot",
  "Maintenance robot",
  "Crew droid",
  "Sentinel droid",
  "Battle droid",
  "Security droid",
  "Command Cyborg",
  "Peaceful Human",
  NULL
};

char *Classes[] = {
  "influence",
  "disposal",
  "servant",
  "messenger",
  "maintenance",
  "crew",
  "sentinel",
  "battle",
  "security",
  "command",
  "error"
};

/*
typedef struct
{
  char* Bonus_name;
  int bonus_to_dex;
  int bonus_to_str;
  int bonus_to_vit;
  int bonus_to_mag;
  int bonus_to_life;
  int bonus_to_force;
  int bonus_to_tohit;
} item_bonus , *Item_bonus;
*/


//    NAME                      DEX  STR  VIT  MAG  HP  MANA  TOHIT ALL AC/DA FIR  ELE  FOR   PRICE-FACTOR
item_bonus PrefixList[] = {
  { "Cheap "                  ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 , 00 , 00 , 00 , 00 ,   0.5 }, // 0
  { "Expensive "              ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 , 00 , 00 , 00 , 00 ,   2.0 }, // 1
  { "Improved "               ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 , 00 , 00 , 00 , 00 ,  40.0 }, // 15
  { "Silver "                 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 , 00 , 00 , 00 , 00 ,  10.0 }, // 3
  { "Platinum "               ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 , 00 , 00 , 00 , 00 ,  10.0 }, // 4 
  { "Heavy "                  ,  0 ,  0 ,  0 ,  0 ,  0 , 15 ,   0 ,   0 , 00 , 00 , 00 , 00 ,  20.0 }, // 5
  { "Light "                  ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   5 ,   0 , 00 , 00 , 00 , 00 ,  20.0 }, // 6
  { "Extra heavy "            ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   5 , 00 , 00 , 00 , 00 ,  20.0 }, // 7
  { "Extra light "            , 10 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 , 00 , 00 , 00 , 00 ,  20.0 }, // 8 
  { "Buggy "                  ,  0 , 10 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 , 00 , 00 , 00 , 00 ,  20.0 }, // 9 
  { "Debugged "               ,  0 ,  0 , 10 ,  0 ,  0 ,  0 ,   0 ,   0 , 00 , 00 , 00 , 00 ,  20.0 }, // 10
  { "Firery "                 ,  0 ,  0 ,  0 , 10 ,  0 ,  0 ,   0 ,   0 , 00 , 00 , 00 , 00 ,  20.0 }, // 11 
  { "Ice cold "               ,  0 ,  0 ,  0 ,  0 , 25 ,  0 ,   0 ,   0 , 00 , 00 , 00 , 00 ,  30.0 }, // 12
  { "Cool "                   ,  0 ,  0 ,  0 ,  0 ,  0 , 25 ,   0 ,   0 , 00 , 00 , 00 , 00 ,  30.0 }, // 13
  { "Hot "                    ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  10 ,   0 , 00 , 00 , 00 , 00 ,  30.0 }, // 14
  { "Elite "                  ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 , 00 , 00 , 00 , 00 ,   3.0 }, // 2 
  { "Opened "                 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 , 00 , 00 , 00 , 00 ,   3.0 }, // 2 
  { "Bugfixed "               ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 , 00 , 00 , 00 , 00 ,   3.0 }, // 2 
  { "Patched "                ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 , 00 , 00 , 00 , 00 ,   3.0 }, // 2 
  { "Severly patched "        ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 , 00 , 00 , 00 , 00 ,   3.0 }, // 2 
  { "Stable "                 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 , 00 , 00 , 00 , 00 ,   3.0 }  // 2 
};

//    NAME                      DEX  STR  VIT  MAG  HP  MANA  TOHIT ALL AC/DA FIR  ELE  FOR   Alvl PRICE-FACTOR
item_bonus SuffixList[] = {
  { " of Closed Source"       ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 , -5 , 00 , 00 , 00 , 00 ,  0 ,  0.1 }, 
  { " of Dexterity"           ,  5 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,  0 , 00 , 00 , 00 , 00 ,  0 , 10.0 }, 
  { " of Strength"            ,  0 ,  5 ,  0 ,  0 ,  0 ,  0 ,   0 ,  0 , 00 , 00 , 00 , 00 ,  0 , 10.0 }, 
  { " of Life"                ,  0 ,  0 ,  5 ,  0 ,  0 ,  0 ,   0 ,  0 , 00 , 00 , 00 , 00 ,  0 , 10.0 }, 
  { " of the Mind"            ,  0 ,  0 ,  0 ,  5 ,  0 ,  0 ,   0 ,  0 , 00 , 00 , 00 , 00 ,  0 , 10.0 }, 
  { " of the Fighter"         ,  0 ,  0 ,  0 , -4 ,  0 ,  0 ,   0 ,  2 , 00 , 00 , 00 , 00 ,  0 , 20.0 }, 
  { " of Life"                ,  0 ,  0 ,  0 ,  0 , 15 ,  0 ,   0 ,  0 , 00 , 00 , 00 , 00 ,  0 , 10.0 }, 
  { " of the Force"           ,  0 ,  0 ,  0 ,  0 ,  0 , 15 ,   0 ,  0 , 00 , 00 , 00 , 00 ,  0 , 20.0 }, 
  { " of the Sniper"          ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   5 ,  0 , 00 , 00 , 00 , 00 ,  0 , 20.0 }, 
  { " of the Open Source"     ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,  5 , 00 , 00 , 00 , 00 ,  0 , 20.0 }, 

  { " of Agility"             , 10 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,  0 , 00 , 00 , 00 , 00 ,  1 , 20.0 }, 
  { " of the Bulldozer"       ,  0 , 10 ,  0 ,  0 ,  0 ,  0 ,   0 ,  0 , 00 , 00 , 00 , 00 ,  1 , 20.0 }, 
  { " of toughness"           ,  0 ,  0 , 10 ,  0 ,  0 ,  0 ,   0 ,  0 , 00 , 00 , 00 , 00 ,  1 , 20.0 }, 
  { " of the Hacker"          ,  0 ,  0 ,  0 , 10 ,  0 ,  0 ,   0 ,  0 , 00 , 00 , 00 , 00 ,  1 , 20.0 }, 
  { " of the Warrior"         ,  0 ,  0 ,  0 , -8 ,  0 ,  0 ,   0 ,  4 , 00 , 00 , 00 , 00 ,  1 , 40.0 }, 
  { " of long life"           ,  0 ,  0 ,  0 ,  0 , 25 ,  0 ,   0 ,  0 , 00 , 00 , 00 , 00 ,  1 , 30.0 }, 
  { " of the spirits"         ,  0 ,  0 ,  0 ,  0 ,  0 , 25 ,   0 ,  0 , 00 , 00 , 00 , 00 ,  1 , 30.0 }, 
  { " of the Guerillias"      ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  10 ,  0 , 00 , 00 , 00 , 00 ,  1 , 30.0 }, 
  { " of the Aladin License"  ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 , 10 , 00 , 00 , 00 , 00 ,  1 , 40.0 },

  { " of Perl scriper"        , 15 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,  0 , 00 , 00 , 00 , 00 ,  2 , 40.0 }, 
  { " of Reiser FS"           ,  0 , 15 ,  0 ,  0 ,  0 ,  0 ,   0 ,  0 , 00 , 00 , 00 , 00 ,  2 , 40.0 }, 
  { " of toughness"           ,  0 ,  0 , 15 ,  0 ,  0 ,  0 ,   0 ,  0 , 00 , 00 , 00 , 00 ,  2 , 40.0 }, 
  { " of the Kernel Hacker"   ,  0 ,  0 ,  0 , 15 ,  0 ,  0 ,   0 ,  0 , 00 , 00 , 00 , 00 ,  2 , 60.0 }, 
  { " of the Knight"          ,  0 ,  0 ,  0 , -8 ,  0 ,  0 ,   0 ,  6 , 00 , 00 , 00 , 00 ,  2 , 40.0 }, 
  { " of long life"           ,  0 ,  0 ,  0 ,  0 , 25 ,  0 ,   0 ,  0 , 00 , 00 , 00 , 00 ,  2 , 60.0 }, 
  { " of the spirits"         ,  0 ,  0 ,  0 ,  0 ,  0 , 25 ,   0 ,  0 , 00 , 00 , 00 , 00 ,  2 , 50.0 }, 
  { " of the Guerillias"      ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  10 ,  0 , 00 , 00 , 00 , 00 ,  2 , 50.0 }, 
  { " of the GPL"             ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 , 10 , 00 , 00 , 00 , 00 ,  2 , 80.0 },

  { " of Perl scriper"        , 15 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,  0 , 00 , 00 , 00 , 00 ,  2 , 40.0 }, 
  { " of Reiser FS"           ,  0 , 15 ,  0 ,  0 ,  0 ,  0 ,   0 ,  0 , 00 , 00 , 00 , 00 ,  2 , 40.0 }, 
  { " of toughness"           ,  0 ,  0 , 15 ,  0 ,  0 ,  0 ,   0 ,  0 , 00 , 00 , 00 , 00 ,  2 , 40.0 }, 
  { " of the Kernel Hacker"   ,  0 ,  0 ,  0 , 15 ,  0 ,  0 ,   0 ,  0 , 00 , 00 , 00 , 00 ,  2 , 60.0 }, 
  { " of the Knight"          ,  0 ,  0 ,  0 , -8 ,  0 ,  0 ,   0 ,  6 , 00 , 00 , 00 , 00 ,  2 , 40.0 }, 
  { " of long life"           ,  0 ,  0 ,  0 ,  0 , 25 ,  0 ,   0 ,  0 , 00 , 00 , 00 , 00 ,  2 , 60.0 }, 
  { " of the Genius"          ,  0 ,  0 ,  0 ,  0 ,  0 , 25 ,   0 ,  0 , 00 , 00 , 00 , 00 ,  2 , 50.0 }, 
  { " of the Guerillias"      ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  10 ,  0 , 00 , 00 , 00 , 00 ,  2 , 50.0 }, 
  { " of the GPL"             ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 , 10 , 00 , 00 , 00 , 00 ,  2 , 80.0 }

};

char *Shipnames[ALLSHIPS] = {
  "Paradroid",
  "Metahawk",
  "Graftgold",
  NULL
};

char *Alertcolor[ALLALERTCOLORS] = {
  "green",
  "yellow",
  "red"
};

char *Drivenames[] = {
  "none",
  "tracks",
  "anti-grav",
  "tripedal",
  "wheels",
  "bipedal",
  "error"
};

char *Sensornames[] = {
  " - ",
  "spectral",
  "infra-red",
  "subsonic",
  "ultra-sonic",
  "radar",
  "error"
};

char *Brainnames[] = {
  "none",
  "neutronic",
  "primode",
  "error"
};

char *Weaponnames[] = {
  "none",
  "lasers",
  "laser rifle",
  "disruptor",
  "exterminator",
  "error"
};

// Robotnummern                 001     123     139     247     249     296     302     329     420     476     493     516     571     598     614     615     629     711     742     751     821     834     883     999

FCU AllFCUs[] = {
  {"none"},
  {"SimpleA"},
  {"Battle1234AD"}
};

Druidspec Druidmap;

Bulletspec Bulletmap;

blastspec Blastmap[ALLBLASTTYPES];



