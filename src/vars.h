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

SDL_Rect Full_Screen_Rect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
SDL_Rect Classic_User_Rect = {
  (SCREEN_WIDTH-9*INITIAL_BLOCK_WIDTH) / 2, 2*(SCREEN_HEIGHT-4*INITIAL_BLOCK_HEIGHT)/3,
  9*INITIAL_BLOCK_WIDTH,  4*INITIAL_BLOCK_HEIGHT
};

// SDL_Rect Full_User_Rect = {0, BANNER_HEIGHT+10, SCREEN_WIDTH, SCREEN_HEIGHT-BANNER_HEIGHT-11};
SDL_Rect Full_User_Rect = {0, BANNER_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT-BANNER_HEIGHT-1};

SDL_Rect Cons_Rect = {16, 162, 595, 315};

SDL_Rect Cons_Menu_Rect = {32, 180, CONS_MENU_LENGTH, CONS_MENU_HEIGHT};
SDL_Rect Cons_Text_Rect = {175, 180, SCREEN_WIDTH-175, 305}; 


int Shieldcolors[INITIAL_BLOCK_WIDTH];

int ShipEmptyCounter = 0;	/* counter to Message: you have won(this ship */

item_image_spec ItemImageList[ NUMBER_OF_ITEM_PICTURES ] = {
  { { 1 , 1 } , NULL } ,  { { 2 , 2 } , NULL } ,  { { 2 , 2 } , NULL } ,  { { 2 , 2 } , NULL } , // 0-3
  { { 2 , 2 } , NULL } ,  { { 2 , 2 } , NULL } ,  { { 2 , 2 } , NULL } ,  { { 1 , 1 } , NULL } , // 4
  { { 2 , 2 } , NULL } ,  { { 2 , 2 } , NULL } ,  { { 2 , 2 } , NULL } ,  { { 2 , 2 } , NULL } , // 8
  { { 2 , 2 } , NULL } ,  { { 2 , 2 } , NULL } ,  { { 2 , 2 } , NULL } ,  { { 2 , 2 } , NULL } , // 12
  { { 2 , 2 } , NULL } ,  { { 1 , 1 } , NULL } ,  { { 2 , 2 } , NULL } ,  { { 2 , 2 } , NULL } , // 16
  { { 2 , 2 } , NULL } ,  { { 2 , 2 } , NULL } ,  { { 1 , 3 } , NULL } ,  { { 2 , 3 } , NULL } , // 20
  { { 2 , 2 } , NULL } ,  { { 2 , 3 } , NULL } ,  { { 2 , 3 } , NULL } ,  { { 1 , 1 } , NULL } , // 24
  { { 1 , 2 } , NULL } ,  { { 1 , 1 } , NULL } ,  { { 1 , 1 } , NULL } ,  { { 2 , 3 } , NULL } , // 28
  { { 2 , 2 } , NULL } ,  { { 2 , 2 } , NULL } ,  { { 1 , 1 } , NULL } ,  { { 1 , 1 } , NULL } , // 32
  { { 2 , 2 } , NULL } ,  { { 2 , 2 } , NULL } ,  { { 2 , 2 } , NULL } ,  { { 2 , 2 } , NULL } , // 36
  { { 2 , 3 } , NULL } ,  { { 1 , 1 } , NULL } ,  { { 1 , 1 } , NULL } ,  { { 2 , 2 } , NULL } , // 40
  { { 2 , 2 } , NULL } ,  { { 2 , 2 } , NULL } ,  { { 2 , 2 } , NULL } ,  { { 2 , 2 } , NULL } , // 44
  { { 1 , 1 } , NULL } ,  { { 1 , 1 } , NULL } ,  { { 1 , 1 } , NULL } ,  { { 1 , 1 } , NULL } , // 48-51
  { { 1 , 1 } , NULL } ,  { { 1 , 1 } , NULL } ,  { { 1 , 1 } , NULL } ,  { { 1 , 1 } , NULL } , // 52-55
  { { 1 , 1 } , NULL } ,  { { 1 , 1 } , NULL } ,  { { 2 , 2 } , NULL } ,  { { 1 , 1 } , NULL }   // 56-59
} ;

/*
influence_t Me[ MAX_PLAYERS ] = {
  { DRUID001, WAR_BOT, TRANSFERMODE, {0, 0}, { 120 , 48 , 0 } , { 120 , 48 , 0 } , { -1 , -1 , -1 } , -1 , 100, 100, 0, 0 },
  { DRUID001, WAR_BOT, TRANSFERMODE, {0, 0}, { 120 , 48 , 0 } , { 120 , 48 , 0 } , { -1 , -1 , -1 } , -1 , 100, 100, 0, 0 },
  { DRUID001, WAR_BOT, TRANSFERMODE, {0, 0}, { 120 , 48 , 0 } , { 120 , 48 , 0 } , { -1 , -1 , -1 } , -1 , 100, 100, 0, 0 },
  { DRUID001, WAR_BOT, TRANSFERMODE, {0, 0}, { 120 , 48 , 0 } , { 120 , 48 , 0 } , { -1 , -1 , -1 } , -1 , 100, 100, 0, 0 },
  { DRUID001, WAR_BOT, TRANSFERMODE, {0, 0}, { 120 , 48 , 0 } , { 120 , 48 , 0 } , { -1 , -1 , -1 } , -1 , 100, 100, 0, 0 }
};
*/

influence_t Me[ MAX_PLAYERS ] = 
{ { DRUID001, WAR_BOT, TRANSFERMODE, {0, 0}, { 120 , 48 , 0 } , { 120 , 48 , 0 } , { -1 , -1 , -1 } , -1 , 100, 100, 0, 0 } };


network_influence_t NetworkMe[ MAX_PLAYERS ];

map_insert_spec AllMapInserts[ MAX_MAP_INSERTS ] = {
  { "BigSquare" , "map_inserts/BigSquare.png" , { 4 , 4 } , NULL } , 
  { "LongLine" , "map_inserts/LongLine.png" , { 4 , 1 } , NULL } ,
  { "TelemetryAnnouncement" , "map_inserts/TelemetryAnnouncement.png" , { 4 , 4 } , NULL } , 
  { "LongRangeTeleporter_Left" , "map_inserts/LongRangeTeleporter_Left.png" , { 3 , 3 } , NULL } ,
  { "LongRangeTeleporter_Right" , "map_inserts/LongRangeTeleporter_Right.png" , { 3 , 3 } , NULL } , 
  { "MaintainanceSquare1x1" , "map_inserts/MaintainanceSquare1x1.png" , { 1 , 1 } , NULL } , 
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
  "Fresh Net",
  "Name Tran",
  NULL
};

char *NetworkClientStatusNames[] = {
  "Network ERROR" ,
  "Unconnected" , 
  "Connection freshly opened" ,
  "Name has been transmitted" , 
  "Game on" ,
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
  "Cold MS Force",
  "Mrs. Stone, Shopowner",
  "Resistance Founder",
  "Town Healer",
  "Sorenson, Master of Magic", // 15
  "Pendragon, Rebel Fighter",
  "Dixon, Teleporter Service Man",
  "Chandra, Tourist Guide",
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

//    NAME                      DEX-DEX   STR-STR   VIT-VIT     MAG-MAG    ALLATT-   HP - HP   MANA-MANA     TOHIT-   AC/DA-   FIR-FIR   ELE-ELE   FOR-FOR    DUMMY Alvl PRICE-FACTOR
item_bonus PrefixList[] = {
  { "Cheap "                  ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,   0 ,   0 , 00 , 00 , 00 , 00 , 00 , 00 , 00 , 00 ,  0 ,    0,     0.5 }, // 0
  { "Expensive "              ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,   0 ,   0 , 00 , 00 , 00 , 00 , 00 , 00 , 00 , 00 ,  0 ,    0,     2.0 }, // 1
  { "Improved "               ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,   0 ,   0 , 00 , 00 , 00 , 00 , 00 , 00 , 00 , 00 ,  0 ,    0,    40.0 }, // 15
  { "Silver "                 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,   0 ,   0 , 00 , 00 , 00 , 00 , 00 , 00 , 00 , 00 ,  0 ,    0,    10.0 }, // 3
  { "Platinum "               ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,   0 ,   0 , 00 , 00 , 00 , 00 , 00 , 00 , 00 , 00 ,  0 ,    0,    10.0 }, // 4 
  { "Heavy "                  ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 ,  0 , 15 , 15 ,   0 ,   0 ,   0 ,   0 , 00 , 00 , 00 , 00 , 00 , 00 , 00 , 00 ,  0 ,    0,    20.0 }, // 5
  { "Light "                  ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 ,  0 ,  0 ,  0 ,   5 ,   5 ,   0 ,   0 , 00 , 00 , 00 , 00 , 00 , 00 , 00 , 00 ,  0 ,    0,    20.0 }, // 6
  { "Extra heavy "            ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,   5 ,   5 , 00 , 00 , 00 , 00 , 00 , 00 , 00 , 00 ,  0 ,    0,    20.0 }, // 7
  { "Extra light "            , 10 , 10 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,   0 ,   0 , 00 , 00 , 00 , 00 , 00 , 00 , 00 , 00 ,  0 ,    0,    20.0 }, // 8 
  { "Buggy "                  ,  0 ,  0 , 10 , 10 ,  0 ,  0 ,   0 ,   0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,   0 ,   0 , 00 , 00 , 00 , 00 , 00 , 00 , 00 , 00 ,  0 ,    0,    20.0 }, // 9 
  { "Debugged "               ,  0 ,  0 ,  0 ,  0 , 10 , 10 ,   0 ,   0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,   0 ,   0 , 00 , 00 , 00 , 00 , 00 , 00 , 00 , 00 ,  0 ,    0,    20.0 }, // 10
  { "Firery "                 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  10 ,  10 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,   0 ,   0 , 00 , 00 , 00 , 00 , 00 , 00 , 00 , 00 ,  0 ,    0,    20.0 }, // 11 
  { "Ice cold "               ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 , 25 , 25 ,  0 ,  0 ,   0 ,   0 ,   0 ,   0 , 00 , 00 , 00 , 00 , 00 , 00 , 00 , 00 ,  0 ,    0,    30.0 }, // 12
  { "Cool "                   ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 ,  0 , 25 , 25 ,   0 ,   0 ,   0 ,   0 , 00 , 00 , 00 , 00 , 00 , 00 , 00 , 00 ,  0 ,    0,    30.0 }, // 13
  { "Hot "                    ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 ,  0 ,  0 ,  0 ,  10 ,  10 ,   0 ,   0 , 00 , 00 , 00 , 00 , 00 , 00 , 00 , 00 ,  0 ,    0,    30.0 }, // 14
  { "Elite "                  ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,   0 ,   0 , 00 , 00 , 00 , 00 , 00 , 00 , 00 , 00 ,  0 ,    0,     3.0 }, // 2 
  { "Opened "                 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,   0 ,   0 , 00 , 00 , 00 , 00 , 00 , 00 , 00 , 00 ,  0 ,    0,     3.0 }, // 2 
  { "Bugfixed "               ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,   0 ,   0 , 00 , 00 , 00 , 00 , 00 , 00 , 00 , 00 ,  0 ,    0,     3.0 }, // 2 
  { "Patched "                ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,   0 ,   0 , 00 , 00 , 00 , 00 , 00 , 00 , 00 , 00 ,  0 ,    0,     3.0 }, // 2 
  { "Severly patched "        ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,   0 ,   0 , 00 , 00 , 00 , 00 , 00 , 00 , 00 , 00 ,  0 ,    0,     3.0 }, // 2 
  { "Stable "                 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,   0 ,   0 , 00 , 00 , 00 , 00 , 00 , 00 , 00 , 00 ,  0 ,    0,     3.0 }  // 2 
};

//    NAME                      DEX-DEX   STR-STR   VIT-VIT      MAG-MAG  ALLATT-    HP - HP   MANA-MANA  TOHIT-    AC/DA-   FIR-FIR   ELE-ELE   FOR-FOR   DUMMY Alvl PRICE-FACTOR
item_bonus SuffixList[] = {
  { " of Closed Source"       ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 , -5 , 0 , -5 , 00 , 00 , 00 , 00 , 00 , 00 , 00 ,  0 ,  0 ,  0.1 }, 
  { " of Dexterity"           ,  5 ,  5 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 , 0 ,  0 , 00 , 00 , 00 , 00 , 00 , 00 , 00 ,  0 ,  0 , 10.0 }, 
  { " of Strength"            ,  0 ,  0 ,  5 ,  5 ,  0 ,  0 ,   0 ,   0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 , 0 ,  0 , 00 , 00 , 00 , 00 , 00 , 00 , 00 ,  0 ,  0 , 10.0 }, 
  { " of Life"                ,  0 ,  0 ,  0 ,  0 ,  5 ,  5 ,   0 ,   0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 , 0 ,  0 , 00 , 00 , 00 , 00 , 00 , 00 , 00 ,  0 ,  0 , 10.0 }, 
  { " of the Mind"            ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   5 ,   5 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 , 0 ,  0 , 00 , 00 , 00 , 00 , 00 , 00 , 00 ,  0 ,  0 , 10.0 }, 
  { " of the Fighter"         ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  -4 ,  -4 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  2 , 0 ,  2 , 00 , 00 , 00 , 00 , 00 , 00 , 00 ,  0 ,  0 , 20.0 }, 
  { " of the man pages"                ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 , 15 , 15 ,  0 ,  0 ,   0 ,   0 ,  0 , 0 ,  0 , 00 , 00 , 00 , 00 , 00 , 00 , 00 ,  0 ,  0 , 10.0 }, 
  { " of the Hack"           ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 ,  0 , 15 , 15 ,   0 ,   0 ,  0 , 0 ,  0 , 00 , 00 , 00 , 00 , 00 , 00 , 00 ,  0 ,  0 , 20.0 }, 
  { " of Filtering"          ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 ,  0 ,  0 ,  0 ,   5 ,   5 ,  0 , 0 ,  0 , 00 , 00 , 00 , 00 , 00 , 00 , 00 ,  0 ,  0 , 20.0 }, 
  { " of the Open Source"     ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  5 , 0 ,  5 , 00 , 00 , 00 , 00 , 00 , 00 , 00 ,  0 ,  0 , 20.0 }, 

  { " of Agility"             , 10 , 10 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 , 0 , 0 , 00 , 00 , 00 , 00 , 00 , 00 , 00 ,  0 ,  1 , 20.0 }, 
  { " of Perl Power"       ,  0 ,  0 , 10 , 10 ,  0 ,  0 ,   0 ,   0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 , 0 , 0 , 00 , 00 , 00 , 00 , 00 , 00 , 00 ,  0 ,  1 , 20.0 }, 
  { " of toughness"           ,  0 ,  0 ,  0 ,  0 , 10 , 10 ,   0 ,   0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 , 0 , 0 , 00 , 00 , 00 , 00 , 00 , 00 , 00 ,  0 ,  1 , 20.0 }, 
  { " of the Hacker"          ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  10 ,  10 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 , 0 , 0 , 00 , 00 , 00 , 00 , 00 , 00 , 00 ,  0 ,  1 , 20.0 }, 
  { " of the Warrior"         ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  -8 ,  -8 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  4 , 0 , 4 , 00 , 00 , 00 , 00 , 00 , 00 , 00 ,  0 ,  1 , 40.0 }, 
  { " of the Howtos"           ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 , 25 , 25 ,  0 ,  0 ,   0 ,   0 ,  0 , 0 , 0 , 00 , 00 , 00 , 00 , 00 , 00 , 00 ,  0 ,  1 , 30.0 }, 
  { " of the spirits"         ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 ,  0 , 25 , 25 ,   0 ,   0 ,  0 , 0 , 0 , 00 , 00 , 00 , 00 , 00 , 00 , 00 ,  0 ,  1 , 30.0 }, 
  { " of Piping"      ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 ,  0 ,  0 ,  0 ,  10 ,  10 ,  0 , 0 , 0 , 00 , 00 , 00 , 00 , 00 , 00 , 00 ,  0 ,  1 , 30.0 }, 
  { " of the Aladin License"  ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 , 10 , 0 , 10 , 00 , 00 , 00 , 00 , 00 , 00 , 00 ,  0 ,  1 , 40.0 },

  { " of Perl scriper"        , 15 , 15 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 , 0 ,  0 , 00 , 00 , 00 , 00 , 00 , 00 , 00 ,  0 ,  2 , 40.0 }, 
  { " of Reiser FS"           ,  0 ,  0 , 15 , 15 ,  0 ,  0 ,   0 ,   0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 , 0 ,  0 , 00 , 00 , 00 , 00 , 00 , 00 , 00 ,  0 ,  2 , 40.0 }, 
  { " of toughness"           ,  0 ,  0 ,  0 ,  0 , 15 , 15 ,   0 ,   0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 , 0 ,  0 , 00 , 00 , 00 , 00 , 00 , 00 , 00 ,  0 ,  2 , 40.0 }, 
  { " of the Kernel Hacker"   ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  15 ,  15 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 , 0 ,  0 , 00 , 00 , 00 , 00 , 00 , 00 , 00 ,  0 ,  2 , 60.0 }, 
  { " of the Knight"          ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  -8 ,  -8 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  6 , 0 ,  6 , 00 , 00 , 00 , 00 , 00 , 00 , 00 ,  0 ,  2 , 40.0 },  
  { " of the info documentation"           ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 , 25 , 25 ,  0 ,  0 ,   0 ,   0 ,  0 , 0 ,  0 , 00 , 00 , 00 , 00 , 00 , 00 , 00 ,  0 ,  2 , 60.0 }, 
  { " of the spirits"         ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 ,  0 , 25 , 25 ,   0 ,   0 ,  0 , 0 ,  0 , 00 , 00 , 00 , 00 , 00 , 00 , 00 ,  0 ,  2 , 50.0 }, 
  { " of the Redirector"      ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 ,  0 ,  0 ,  0 ,  10 ,  10 ,  0 , 0 ,  0 , 00 , 00 , 00 , 00 , 00 , 00 , 00 ,  0 ,  2 , 50.0 }, 
  { " of the GPL"             ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 , 10 , 0 , 10 , 00 , 00 , 00 , 00 , 00 , 00 , 00 ,  0 ,  2 , 80.0 },

  { " of Perl scriper"        , 15 , 15 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 , 0 ,  0 , 00 , 00 , 00 , 00 , 00 , 00 , 00 ,  0 ,  2 , 40.0 }, 
  { " of Reiser FS"           ,  0 ,  0 , 15 , 15 ,  0 ,  0 ,   0 ,   0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 , 0 ,  0 , 00 , 00 , 00 , 00 , 00 , 00 , 00 ,  0 ,  2 , 40.0 }, 
  { " of toughness"           ,  0 ,  0 ,  0 ,  0 , 15 , 15 ,   0 ,   0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 , 0 ,  0 , 00 , 00 , 00 , 00 , 00 , 00 , 00 ,  0 ,  2 , 40.0 }, 
  { " of the Kernel Hacker"   ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  15 ,  15 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 , 0 ,  0 , 00 , 00 , 00 , 00 , 00 , 00 , 00 ,  0 ,  2 , 60.0 }, 
  { " of the Knight"          ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  -8 ,  -8 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  6 , 0 ,  6 , 00 , 00 , 00 , 00 , 00 , 00 , 00 ,  0 ,  2 , 40.0 }, 
  { " of the commented source"           ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 , 25 , 25 ,  0 ,  0 ,   0 ,   0 ,  0 , 0 ,  0 , 00 , 00 , 00 , 00 , 00 , 00 , 00 ,  0 ,  2 , 60.0 }, 
  { " of the Genius"          ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 ,  0 , 25 , 25 ,   0 ,   0 ,  0 , 0 ,  0 , 00 , 00 , 00 , 00 , 00 , 00 , 00 ,  0 ,  2 , 50.0 }, 
  { " of the Guerillias"      ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 ,  0 ,  0 ,  0 ,  10 ,  10 ,  0 , 0 ,  0 , 00 , 00 , 00 , 00 , 00 , 00 , 00 ,  0 ,  2 , 50.0 }, 
  { " of the GPL"             ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 , 10 , 0 , 10 , 00 , 00 , 00 , 00 , 00 , 00 , 00 ,  0 ,  2 , 80.0 },

  { " of the LDP Project"           ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 , 35 , 35 ,  0 ,  0 ,   0 ,   0 ,  0 , 0 ,  0 , 00 , 00 , 00 , 00 , 00 , 00 , 00 ,  0 ,  2 , 80.0 }

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

char* AllSkillTexts [ ] =
  {
    "novice",
    "average",
    "experienced",
    "skilled",
    "adept",
    "masterful",
    "inhuman",
    "god-like",
    "super-god-like",
    "ERROR/UNDEFINED SKILL_LEVEL"
  };

//--------------------
// This table indicates how many points of magic energy ('force')
// it takes to cast a given spell, provided the caster is of 
// the given skill level.
//
int ManaCostTable [ NUMBER_OF_SKILLS ] [ NUMBER_OF_SKILL_LEVELS ] = {
  {    0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 } , // takeover skill.  This should remain free
  {    0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 } , // 'Repair Skill'
  {    6 ,  5 ,  4 ,  3 ,  2 ,  1 ,  1 ,  1 ,  1 ,  1 } , // 'Explosion ray'
  {    6 ,  5 ,  4 ,  3 ,  2 ,  1 ,  1 ,  1 ,  1 ,  1 } , // 'Healing'
  {    6 ,  5 ,  4 ,  3 ,  2 ,  1 ,  1 ,  1 ,  1 ,  1 } , // 'Teleportal Home'
  {    6 ,  5 ,  4 ,  3 ,  2 ,  1 ,  1 ,  1 ,  1 ,  1 } , // 'Firey Bolt'
  {    6 ,  5 ,  4 ,  3 ,  2 ,  1 ,  1 ,  1 ,  1 ,  1 } , // 'Ice Bolt'
  {    6 ,  5 ,  4 ,  3 ,  2 ,  1 ,  1 ,  1 ,  1 ,  1 } , // 'Explosion circle'
  {    6 ,  5 ,  4 ,  3 ,  2 ,  1 ,  1 ,  1 ,  1 ,  1 } , // 'Poison Bolt'
  {    6 ,  5 ,  4 ,  3 ,  2 ,  1 ,  1 ,  1 ,  1 ,  1 } , // 'Petrification'
  {    6 ,  5 ,  4 ,  3 ,  2 ,  1 ,  1 ,  1 ,  1 ,  1 } , // 'Detect Stuff'
  {    6 ,  5 ,  4 ,  3 ,  2 ,  1 ,  1 ,  1 ,  1 ,  1 } , // 'EMP Radial Wave'
  {    6 ,  5 ,  4 ,  3 ,  2 ,  1 ,  1 ,  1 ,  1 ,  1 } , // 'VMS Radial Wave'
  {    6 ,  5 ,  4 ,  3 ,  2 ,  1 ,  1 ,  1 ,  1 ,  1 } , // 'UNUSED SLOT 13'
  {    6 ,  5 ,  4 ,  3 ,  2 ,  1 ,  1 ,  1 ,  1 ,  1 } , // 'UNUSED SLOT 14'
  {    6 ,  5 ,  4 ,  3 ,  2 ,  1 ,  1 ,  1 ,  1 ,  1 } , // 'UNUSED SLOT 15'
  {    6 ,  5 ,  4 ,  3 ,  2 ,  1 ,  1 ,  1 ,  1 ,  1 } , // 'UNUSED SLOT 16'
  {    6 ,  5 ,  4 ,  3 ,  2 ,  1 ,  1 ,  1 ,  1 ,  1 } , // 'UNUSED SLOT 17'
  {    6 ,  5 ,  4 ,  3 ,  2 ,  1 ,  1 ,  1 ,  1 ,  1 } , // 'UNUSED SLOT 18'
  {    6 ,  5 ,  4 ,  3 ,  2 ,  1 ,  1 ,  1 ,  1 ,  1 } , // 'UNUSED SLOT 19'
  {    6 ,  5 ,  4 ,  3 ,  2 ,  1 ,  1 ,  1 ,  1 ,  1 } , // 'UNUSED SLOT 20'
  {    6 ,  5 ,  4 ,  3 ,  2 ,  1 ,  1 ,  1 ,  1 ,  1 } , // 'UNUSED SLOT 21'
  {    6 ,  5 ,  4 ,  3 ,  2 ,  1 ,  1 ,  1 ,  1 ,  1 } , // 'UNUSED SLOT 22'
};

//--------------------
// When a character aquires better melee_weapon_skill for melee
// weapons or better ranged_weapon_skill for ranged weapons, this
// will affect (his chance to hit and also) the damage the player
// does, by applying a multiplier to the normal computed damage.
// These multipliers are given by the table below.
//
float MeleeDamageMultiplierTable [  ] =
  {  1.0 , 1.5 , 2.0 , 2.5 , 3.0 , 3.5 , 4.0 , 4.5 , 5.0 } ;
float MeleeRechargeMultiplierTable [  ] =
  {  1.0 , 0.8 , 0.6 , 0.5 , 0.4 , 0.2 , 0.15 , 0.10 , 0.05 } ;

float RangedDamageMultiplierTable [  ] =
  {  1.0 , 1.5 , 2.0 , 2.5 , 3.0 , 3.5 , 4.0 , 4.5 , 5.0} ;
float RangedRechargeMultiplierTable [  ] =
  {  1.0 , 0.8 , 0.6 , 0.5 , 0.4 , 0.2 , 0.15 , 0.10 , 0.05 } ;

//--------------------
// In the game, many spells are still internally similar
// to bullets flying around.  But what hit chance shall this
// bullet have?  --  That will depend upon the skill level
// of the caster.  The details will be taken from the following
// table:
//
int SpellHitPercentageTable [  ] =
  {  50 , 70 , 90 , 110 , 130 , 150 , 190 , 230 , 270 } ;

char* SkillName[ NUMBER_OF_SKILLS ] = {

  "Takeover/Talk Skill",
  "Force Explosion Circle",
  "Force Explosion Ray",
  "Force -> Energy ",
  "Create Teleportal Home",

  "Firey Bolt", // "Remote Strike",
  "Ice Bolt",
  "Unused spell Nr. 7" , // "Mana Shield",
  "Poison Bolt",
  "Petrification",

  "Detect Stuff",
  "EMP Radial Wave",
  "Unused spell Nr. 12",
  "Unused spell Nr. 13",
  "Unused spell Nr. 14",
  
  "Unused spell Nr. 15",
  "Unused spell Nr. 16",
  "Unused spell Nr. 17",
  "Unused spell Nr. 18",
  "Unused spell Nr. 19",
  
  "Unused spell Nr. 20",
  "Unused spell Nr. 21",
  "Unused spell Nr. 22",
  "Unused spell Nr. 23",
  "Unused spell Nr. 24",
  
  "Unused spell Nr. 25",
  "Unused spell Nr. 26",
  "Unused spell Nr. 27",
  "Unused spell Nr. 28",
  "Unused spell Nr. 29",
  
  "Unused spell Nr. 30",
  "Unused spell Nr. 31",
  "Unused spell Nr. 32",
  "Unused spell Nr. 33",
  "Unused spell Nr. 34",
  
  "Unused spell Nr. 35",
  "Unused spell Nr. 36",
  "Unused spell Nr. 37",
  "Unused spell Nr. 38",
  "Unused spell Nr. 39",
  
  "Unused spell Nr. 40",
  "Unused spell Nr. 41",
  "Unused spell Nr. 42",
  "Unused spell Nr. 43",
  "Unused spell Nr. 44",
  
  "Unused spell Nr. 45",
  "Unused spell Nr. 46",
  "Unused spell Nr. 47",
  "Unused spell Nr. 48",
  "Unused spell Nr. 49"
  
};

Druidspec Druidmap;

Bulletspec Bulletmap;

blastspec Blastmap[ALLBLASTTYPES];



