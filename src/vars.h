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

SDL_Rect Cons_Text_Rect = {175, 180, SCREEN_WIDTH-175, 305}; 

/*
  iso_image image;
  int block_area_type;
  float block_area_parm_1;
  float block_area_parm_2;
  int is_smashable;
  int needs_pre_put; // this is a special property for obstacles, that can be stepped on, like a rug or floor plate.
  int drop_random_treasure;
  char* filename;
*/

char* floor_tile_filenames [ ALL_ISOMETRIC_FLOOR_TILES ] =
  {
    "iso_miscellaneous_floor_0000.png" , 
    "iso_miscellaneous_floor_0001.png" , 
    "iso_miscellaneous_floor_0002.png" , 
    "iso_miscellaneous_floor_0003.png" , 
    "iso_miscellaneous_floor_0004.png" , 
    "iso_miscellaneous_floor_0005.png" , 
    "iso_grass_floor_0000.png" , 
    "iso_grass_floor_0001.png" , 
    "iso_grass_floor_0002.png" , 
    "iso_grass_floor_0003.png" , 
    "iso_grass_floor_0004.png" , 
    "iso_grass_floor_0005.png" , 
    "iso_grass_floor_0006.png" , 
    "iso_grass_floor_0007.png" , 
    "iso_grass_floor_0008.png" , 
    "iso_grass_floor_0009.png" , 
    "iso_grass_floor_0010.png" , 
    "iso_grass_floor_0011.png" , 
    "iso_grass_floor_0012.png" , 
    "iso_grass_floor_0013.png" , 
    "iso_grass_floor_0014.png" , 
    "iso_grass_floor_0015.png" , 
    "iso_grass_floor_0016.png" , 
    "iso_grass_floor_0017.png" , 
    "iso_grass_floor_0018.png" , 
    "iso_grass_floor_0019.png" , 
    "iso_grass_floor_0020.png" , 
    "iso_grass_floor_0021.png" , 
    "iso_grass_floor_0022.png" , 
    "iso_grass_floor_0023.png" , 
    "iso_miscellaneous_floor_0006.png" , 
    "iso_miscellaneous_floor_0007.png" , 
    "iso_miscellaneous_floor_0008.png" , 
    "iso_grass_floor_0024.png" , 
    "iso_grass_floor_0025.png" ,
    "iso_grass_floor_0026.png" , 
    "iso_grass_floor_0027.png" , 
    "iso_grass_floor_0028.png" ,

    "iso_miscellaneous_floor_0009.png" ,
    "iso_miscellaneous_floor_0010.png" ,
    "iso_miscellaneous_floor_0011.png" ,
    "iso_miscellaneous_floor_0012.png" ,
    "iso_miscellaneous_floor_0013.png" ,
    "iso_miscellaneous_floor_0014.png" ,
    "iso_miscellaneous_floor_0015.png" ,
    "iso_miscellaneous_floor_0016.png" ,
    "iso_miscellaneous_floor_0017.png" ,
    "iso_miscellaneous_floor_0018.png" ,
    "iso_miscellaneous_floor_0019.png" ,
    "iso_miscellaneous_floor_0020.png" ,
    "iso_miscellaneous_floor_0021.png" ,
    "iso_miscellaneous_floor_0022.png" 

    // , "ERROR_UNUSED.png" , 
  };

/*
tux_t Me[ MAX_PLAYERS ] = {
  { DRUID001, WAR_BOT, TRANSFERMODE, {0, 0}, { 120 , 48 , 0 } , { 120 , 48 , 0 } , { -1 , -1 , -1 } , -1 , 100, 100, 0, 0 },
  { DRUID001, WAR_BOT, TRANSFERMODE, {0, 0}, { 120 , 48 , 0 } , { 120 , 48 , 0 } , { -1 , -1 , -1 } , -1 , 100, 100, 0, 0 },
  { DRUID001, WAR_BOT, TRANSFERMODE, {0, 0}, { 120 , 48 , 0 } , { 120 , 48 , 0 } , { -1 , -1 , -1 } , -1 , 100, 100, 0, 0 },
  { DRUID001, WAR_BOT, TRANSFERMODE, {0, 0}, { 120 , 48 , 0 } , { 120 , 48 , 0 } , { -1 , -1 , -1 } , -1 , 100, 100, 0, 0 },
  { DRUID001, WAR_BOT, TRANSFERMODE, {0, 0}, { 120 , 48 , 0 } , { 120 , 48 , 0 } , { -1 , -1 , -1 } , -1 , 100, 100, 0, 0 }
};
*/

tux_t Me[ MAX_PLAYERS ] = 
{ { DRUID001, WAR_BOT, TRANSFERMODE, {0, 0}, { 120 , 48 , 0 } , { 120 , 48 , 0 } , { -1 , -1 , -1 } , -1 , 100, 100, 0, 0 } };


network_tux_t NetworkMe[ MAX_PLAYERS ];

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

//    NAME                      DEX-DEX   STR-STR   VIT-VIT     MAG-MAG    
//    ALLATT-   HP - HP   MANA-MANA     TOHIT-   AC/DA-   FIR-FIR   
//    ELE-ELE   FOR-FOR    Light Alvl PRICE-FACTOR
item_bonus PrefixList[] = {
    { "Cheap "                  ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  
      0 ,  0 ,  0 ,  0 ,   0 ,   0 ,   0 ,   0 , 00 , 00 , 00 , 00 , 
      00 , 00 , 00 , 00 ,  0 ,    0,     0.5 }, 
    
    { "Expensive "              ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,
      0 ,  0 ,  0 ,  0 ,   0 ,   0 ,   0 ,   0 , 00 , 00 , 00 , 00 , 
      00 , 00 , 00 , 00 ,  0 ,    0,     1.5 }, 
    
    { "Glowing "                ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,
      0 ,  0 ,  0 ,  0 ,   0 ,   0 ,   0 ,   0 , 00 , 00 , 00 , 00 , 
      00 , 00 , 00 , 00 ,  +2,    0,     0.5 }, 
    
    { "Improved "               ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,
      0 ,  0 ,  0 ,  0 ,   0 ,   0 ,   0 ,   0 , 00 , 00 , 00 , 00 , 
      00 , 00 , 00 , 00 ,  0 ,    0,     5.0 }, 
    
    { "Silver "                 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,
      0 ,  0 ,  0 ,  0 ,   0 ,   0 ,   0 ,   0 , 00 , 00 , 00 , 00 , 
      00 , 00 , 00 , 00 ,  0 ,    0,     2.0 }, 
    
    { "Platinum "               ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,
      0 ,  0 ,  0 ,  0 ,   0 ,   0 ,   0 ,   0 , 00 , 00 , 00 , 00 , 
      00 , 00 , 00 , 00 ,  0 ,    0,     2.0 }, 
    
    { "Heavy "                  ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,
      0 ,  0 , 15 , 15 ,   0 ,   0 ,   0 ,   0 , 00 , 00 , 00 , 00 , 
      00 , 00 , 00 , 00 ,  0 ,    0,     3.0 }, 
    
    { "Radiating "              ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,
      0 ,  0 ,  0 ,  0 ,   0 ,   0 ,   0 ,   0 , 00 , 00 , 00 , 00 , 
      00 , 00 , 00 , 00 ,  +4,    0,     0.5 }, 
    
    { "Light "                  ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,
      0 ,  0 ,  0 ,  0 ,   5 ,   5 ,   0 ,   0 , 00 , 00 , 00 , 00 , 
      00 , 00 , 00 , 00 ,  0 ,    0,     3.0 }, 
    
    { "Extra heavy "            ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,
      0 ,  0 ,  0 ,  0 ,   0 ,   0 ,   5 ,   5 , 00 , 00 , 00 , 00 , 
      00 , 00 , 00 , 00 ,  0 ,    0,     3.0 }, 
    
    { "Extra light "            , 10 , 10 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,
      0 ,  0 ,  0 ,  0 ,   0 ,   0 ,   0 ,   0 , 00 , 00 , 00 , 00 , 
      00 , 00 , 00 , 00 ,  0 ,    0,     3.0 }, 

    { "Buggy "                  ,  0 ,  0 , 10 , 10 ,  0 ,  0 ,   0 ,   0 ,
      0 ,  0 ,  0 ,  0 ,   0 ,   0 ,   0 ,   0 , 00 , 00 , 00 , 00 , 
      00 , 00 , 00 , 00 ,  0 ,    0,     3.0 }, 
    
    { "Debugged "               ,  0 ,  0 ,  0 ,  0 , 10 , 10 ,   0 ,   0 ,
      0 ,  0 ,  0 ,  0 ,   0 ,   0 ,   0 ,   0 , 00 , 00 , 00 , 00 , 
      00 , 00 , 00 , 00 ,  0 ,    0,     3.0 }, 
    
    { "Firery "                 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  10 ,  10 ,
      0 ,  0 ,  0 ,  0 ,   0 ,   0 ,   0 ,   0 , 00 , 00 , 00 , 00 , 
      00 , 00 , 00 , 00 ,  0 ,    0,     3.0 }, 
    
    { "Ice cold "               ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,
      25 , 25 ,  0 ,  0 ,   0 ,   0 ,   0 ,   0 , 00 , 00 , 00 , 00 , 
      00 , 00 , 00 , 00 ,  0 ,    0,     4.0 }, 
    
    { "Cool "                   ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,
      0 ,  0 , 25 , 25 ,   0 ,   0 ,   0 ,   0 , 00 , 00 , 00 , 00 , 
      00 , 00 , 00 , 00 ,  0 ,    0,     4.0 }, 
    
    { "Hot "                    ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,
      0 ,  0 ,  0 ,  0 ,  10 ,  10 ,   0 ,   0 , 00 , 00 , 00 , 00 , 
      00 , 00 , 00 , 00 ,  0 ,    0,     4.0 }, 
    
    { "Elite "                  ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,
      0 ,  0 ,  0 ,  0 ,   0 ,   0 ,   0 ,   0 , 00 , 00 , 00 , 00 , 
      00 , 00 , 00 , 00 ,  0 ,    0,     5.0 }, 
    
    { "Opened "                 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,
      0 ,  0 ,  0 ,  0 ,   0 ,   0 ,   0 ,   0 , 00 , 00 , 00 , 00 , 
      00 , 00 , 00 , 00 ,  0 ,    0,     4.0 }, 
    
    { "Bugfixed "               ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,
      0 ,  0 ,  0 ,  0 ,   0 ,   0 ,   0 ,   0 , 00 , 00 , 00 , 00 , 
      00 , 00 , 00 , 00 ,  0 ,    0,     4.0 }, 
    
    { "Patched "                ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,
      0 ,  0 ,  0 ,  0 ,   0 ,   0 ,   0 ,   0 , 00 , 00 , 00 , 00 , 
      00 , 00 , 00 , 00 ,  0 ,    0,     3.0 }, 
    
    { "Severly patched "        ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,
      0 ,  0 ,  0 ,  0 ,   0 ,   0 ,   0 ,   0 , 00 , 00 , 00 , 00 , 
      00 , 00 , 00 , 00 ,  0 ,    0,     3.0 }, 
    
    { "Stable "                 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,
      0 ,  0 ,  0 ,  0 ,   0 ,   0 ,   0 ,   0 , 00 , 00 , 00 , 00 , 
      00 , 00 , 00 , 00 ,  0 ,    0,     3.0 }  ,
    
    { "*** END OF PREFIX LIST ***"                 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,
      0 ,  0 ,  0 ,  0 ,   0 ,   0 ,   0 ,   0 , 00 , 00 , 00 , 00 , 
      00 , 00 , 00 , 00 ,  0 ,    0,     1.0 }  

};

//    NAME                      DEX-DEX   STR-STR   VIT-VIT      MAG-MAG  
// ALLATT-    HP - HP   MANA-MANA  TOHIT-    AC/DA-   FIR-FIR   
// ELE-ELE   FOR-FOR   DUMMY Alvl PRICE-FACTOR
item_bonus SuffixList[] = {
  { " of Dexterity"           ,  1 ,  1 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  
    0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 , 0 ,  0 , 00 , 00 , 00 , 
    00 , 00 , 00 , 00 ,  0 ,  0 ,  2.0 }, 
  { " of Strength"            ,  0 ,  0 ,  1 ,  1 ,  0 ,  0 ,   0 ,   0 ,  
    0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 , 0 ,  0 , 00 , 00 , 00 , 
    00 , 00 , 00 , 00 ,  0 ,  0 ,  2.0 }, 
  { " of Life"                ,  0 ,  0 ,  0 ,  0 ,  1 ,  1 ,   0 ,   0 ,  
    0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 , 0 ,  0 , 00 , 00 , 00 , 
    00 , 00 , 00 , 00 ,  0 ,  0 ,  2.0 }, 
  { " of the Mind"            ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   1 ,   1 ,  
    0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 , 0 ,  0 , 00 , 00 , 00 , 
    00 , 00 , 00 , 00 ,  0 ,  0 ,  2.0 }, 
  { " of Consistency"         ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   1 ,   1 ,  
    0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 , 0 ,  0 , 00 , 00 , 00 , 
    05 , 10 , 00 , 00 ,  0 ,  0 ,  2.0 }, 
  { " of Dexterity"           ,  2 ,  2 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  
    0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 , 0 ,  0 , 00 , 00 , 00 , 
    00 , 00 , 00 , 00 ,  0 ,  0 ,  3.0 }, 
  { " of Strength"            ,  0 ,  0 ,  2 ,  2 ,  0 ,  0 ,   0 ,   0 ,  
    0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 , 0 ,  0 , 00 , 00 , 00 , 
    00 , 00 , 00 , 00 ,  0 ,  0 ,  3.0 }, 
  { " of Life"                ,  0 ,  0 ,  0 ,  0 ,  2 ,  2 ,   0 ,   0 ,  
    0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 , 0 ,  0 , 00 , 00 , 00 , 
    00 , 00 , 00 , 00 ,  0 ,  0 ,  3.0 }, 
  { " of the Mind"            ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   2 ,   2 ,  
    0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 , 0 ,  0 , 00 , 00 , 00 , 
    00 , 00 , 00 , 00 ,  0 ,  0 ,  3.0 }, 
  { " of Completeness"         ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   1 ,   1 ,  
    0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 , 0 ,  0 , 00 , 00 , 00 , 
    15 , 20 , 00 , 00 ,  0 ,  0 ,  5.0 }, 
  { " of Dexterity"           ,  3 ,  2 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  
    0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 , 0 ,  0 , 00 , 00 , 00 , 
    00 , 00 , 00 , 00 ,  0 ,  0 ,  4.0 }, 
  { " of Strength"            ,  0 ,  0 ,  3 ,  2 ,  0 ,  0 ,   0 ,   0 ,  
    0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 , 0 ,  0 , 00 , 00 , 00 , 
    00 , 00 , 00 , 00 ,  0 ,  0 ,  4.0 }, 
  { " of Life"                ,  0 ,  0 ,  0 ,  0 ,  3 ,  2 ,   0 ,   0 ,  
    0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 , 0 ,  0 , 00 , 00 , 00 , 
    00 , 00 , 00 , 00 ,  0 ,  0 ,  4.0 }, 
  { " of the Mind"            ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   3 ,   2 ,  
    0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 , 0 ,  0 , 00 , 00 , 00 , 
    00 , 00 , 00 , 00 ,  0 ,  0 ,  4.0 }, 
  { " of Compactness"         ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   1 ,   1 ,  
    0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 , 0 ,  0 , 00 , 00 , 00 , 
    25 , 30 , 00 , 00 ,  0 ,  0 , 10.0 }, 
  { " of Dexterity"           ,  4 ,  2 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  
    0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 , 0 ,  0 , 00 , 00 , 00 , 
    00 , 00 , 00 , 00 ,  0 ,  0 ,  5.0 }, 
  { " of Strength"            ,  0 ,  0 ,  4 ,  2 ,  0 ,  0 ,   0 ,   0 ,  
    0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 , 0 ,  0 , 00 , 00 , 00 , 
    00 , 00 , 00 , 00 ,  0 ,  0 ,  5.0 }, 
  { " of Life"                ,  0 ,  0 ,  0 ,  0 ,  4 ,  2 ,   0 ,   0 ,  
    0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 , 0 ,  0 , 00 , 00 , 00 , 
    00 , 00 , 00 , 00 ,  0 ,  0 ,  5.0 }, 
  { " of the Mind"            ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   4 ,   2 ,  
    0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 , 0 ,  0 , 00 , 00 , 00 , 
    00 , 00 , 00 , 00 ,  0 ,  0 ,  5.0 }, 
  { " of Dexterity"           ,  5 ,  2 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  
    0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 , 0 ,  0 , 00 , 00 , 00 , 
    00 , 00 , 00 , 00 ,  0 ,  0 ,  7.0 }, 
  { " of Strength"            ,  0 ,  0 ,  5 ,  2 ,  0 ,  0 ,   0 ,   0 ,  
    0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 , 0 ,  0 , 00 , 00 , 00 , 
    00 , 00 , 00 , 00 ,  0 ,  0 ,  7.0 }, 
  { " of Life"                ,  0 ,  0 ,  0 ,  0 ,  5 ,  2 ,   0 ,   0 ,  
    0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 , 0 ,  0 , 00 , 00 , 00 , 
    00 , 00 , 00 , 00 ,  0 ,  0 ,  7.0 }, 
  { " of the Mind"            ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   5 ,   2 ,  
    0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 , 0 ,  0 , 00 , 00 , 00 , 
    00 , 00 , 00 , 00 ,  0 ,  0 ,  7.0 }, 
  { " of Continuity"         ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   1 ,   1 ,  
    0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 , 0 ,  0 , 00 , 00 , 00 , 
    35 , 40 , 00 , 00 ,  0 ,  0 , 15.0 }, 
  { " of the Fighter"         ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  -4 ,  -4 ,  
    0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  2 , 0 ,  2 , 00 , 00 , 00 , 
    00 , 00 , 00 , 00 ,  0 ,  0 ,  8.0 }, 
  { " of the man pages"       ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 , 
    15 , 15 ,  0 ,  0 ,   0 ,   0 ,  0 , 0 ,  0 , 00 , 00 , 00 , 
    00 , 00 , 00 , 00 ,  0 ,  0 , 10.0 }, 
  { " of the Hack"            ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 , 
    0 ,  0 , 15 , 15 ,   0 ,   0 ,  0 , 0 ,  0 , 00 , 00 , 00 , 
    00 , 00 , 00 , 00 ,  0 ,  0 ,  8.0 }, 
  { " of Filtering"           ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  
    0 ,  0 ,  0 ,  0 ,   5 ,   5 ,  0 , 0 ,  0 , 00 , 00 , 00 , 
    00 , 00 , 00 , 00 ,  0 ,  0 ,  8.0 }, 
  { " of the Open Source"     ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  
    0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  5 , 0 ,  5 , 00 , 00 , 00 , 
    00 , 00 , 00 , 00 ,  0 ,  0 , 8.0 }, 

  { " of Agility"             , 10 , 10 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  
    0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 , 0 , 0 , 00 , 00 , 00 , 
    00 , 00 , 00 , 00 ,  0 ,  1 , 8.0 }, 
  { " of Perl Power"          ,  0 ,  0 , 10 , 10 ,  0 ,  0 ,   0 ,   0 ,  
    0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 , 0 , 0 , 00 , 00 , 00 , 
    00 , 00 , 00 , 00 ,  0 ,  1 ,  8.0 }, 
  { " of toughness"           ,  0 ,  0 ,  0 ,  0 , 10 , 10 ,   0 ,   0 ,  
    0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 , 0 , 0 , 00 , 00 , 00 , 
    00 , 00 , 00 , 00 ,  0 ,  1 , 8.0 }, 
  { " of the Hacker"          ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  10 ,  10 ,  
    0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 , 0 , 0 , 00 , 00 , 00 , 
    00 , 00 , 00 , 00 ,  0 ,  1 , 8.0 }, 
  { " of the Warrior"         ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  -8 ,  -8 ,  
    0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  4 , 0 , 4 , 00 , 00 , 00 , 
    00 , 00 , 00 , 00 ,  0 ,  1 , 10.0 }, 
  { " of the Howtos"          ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 , 
    25 , 25 ,  0 ,  0 ,   0 ,   0 ,  0 , 0 , 0 , 00 , 00 , 00 , 
    00 , 00 , 00 , 00 ,  0 ,  1 , 8.0 }, 
  { " of the spirits"         ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  
    0 ,  0 , 25 , 25 ,   0 ,   0 ,  0 , 0 , 0 , 00 , 00 , 00 , 
    00 , 00 , 00 , 00 ,  0 ,  1 ,  8.0 }, 
  { " of Piping"              ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  
    0 ,  0 ,  0 ,  0 ,  10 ,  10 ,  0 , 0 , 0 , 00 , 00 , 00 , 
    00 , 00 , 00 , 00 ,  0 ,  1 ,  8.0 }, 
  { " of the Aladin License"  ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  
    0 ,  0 ,  0 ,  0 ,   0 ,   0 , 10 , 0 , 10 , 00 , 00 , 00 , 
    00 , 00 , 00 , 00 ,  0 ,  1 , 9.0 },

  { " of Perl scriper"        , 15 , 15 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  
    0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 , 0 ,  0 , 00 , 00 , 00 , 
    00 , 00 , 00 , 00 ,  0 ,  2 , 9.0 }, 
  { " of Reiser FS"           ,  0 ,  0 , 15 , 15 ,  0 ,  0 ,   0 ,   0 ,  
    0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 , 0 ,  0 , 00 , 00 , 00 , 
    00 , 00 , 00 , 00 ,  0 ,  2 , 9.0 }, 
  { " of toughness"           ,  0 ,  0 ,  0 ,  0 , 15 , 15 ,   0 ,   0 ,  
    0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 , 0 ,  0 , 00 , 00 , 00 , 
    00 , 00 , 00 , 00 ,  0 ,  2 , 9.0 }, 
  { " of the Kernel Hacker"   ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  15 ,  15 ,  
    0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 , 0 ,  0 , 00 , 00 , 00 , 
    00 , 00 , 00 , 00 ,  0 ,  2 , 12.0 }, 
  { " of the Knight"          ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  -8 ,  -8 ,  
    0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  6 , 0 ,  6 , 00 , 00 , 00 , 
    00 , 00 , 00 , 00 ,  0 ,  2 , 9.0 },  
  { " of the info documentation" , 0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 , 
    25 , 25 ,  0 ,  0 ,   0 ,   0 ,  0 , 0 ,  0 , 00 , 00 , 00 , 
    00 , 00 , 00 , 00 ,  0 ,  2 , 60.0 }, 
  { " of the spirits"         ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  
    0 ,  0 , 25 , 25 ,   0 ,   0 ,  0 , 0 ,  0 , 00 , 00 , 00 , 
    00 , 00 , 00 , 00 ,  0 ,  2 , 10.0 }, 
  { " of the Redirector"      ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  
    0 ,  0 ,  0 ,  0 ,  10 ,  10 ,  0 , 0 ,  0 , 00 , 00 , 00 , 
    00 , 00 , 00 , 00 ,  0 ,  2 , 10.0 }, 
  { " of the GPL"             ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  
    0 ,  0 ,  0 ,  0 ,   0 ,   0 , 10 , 0 , 10 , 00 , 00 , 00 , 
    00 , 00 , 00 , 00 ,  0 ,  2 , 14.0 },

  { " of Perl scriper"        , 15 , 15 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  
    0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 , 0 ,  0 , 00 , 00 , 00 , 
    00 , 00 , 00 , 00 ,  0 ,  2 , 12.0 }, 
  { " of Reiser FS"           ,  0 ,  0 , 15 , 15 ,  0 ,  0 ,   0 ,   0 ,  
    0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 , 0 ,  0 , 00 , 00 , 00 , 
    00 , 00 , 00 , 00 ,  0 ,  2 , 10.0 }, 
  { " of toughness"           ,  0 ,  0 ,  0 ,  0 , 15 , 15 ,   0 ,   0 ,  
    0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 , 0 ,  0 , 00 , 00 , 00 , 
    00 , 00 , 00 , 00 ,  0 ,  2 , 10.0 }, 
  { " of the Kernel Hacker"   ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  15 ,  15 ,  
    0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  0 , 0 ,  0 , 00 , 00 , 00 , 
    00 , 00 , 00 , 00 ,  0 ,  2 , 14.0 }, 
  { " of the Knight"          ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  -8 ,  -8 ,  
    0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  6 , 0 ,  6 , 00 , 00 , 00 , 
    00 , 00 , 00 , 00 ,  0 ,  2 , 10.0 }, 
  { " of the commented source" ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 , 
    25 , 25 ,  0 ,  0 ,   0 ,   0 ,  0 , 0 ,  0 , 00 , 00 , 00 , 
    00 , 00 , 00 , 00 ,  0 ,  2 , 60.0 }, 
  { " of the Genius"          ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  
    0 ,  0 , 25 , 25 ,   0 ,   0 ,  0 , 0 ,  0 , 00 , 00 , 00 , 
    00 , 00 , 00 , 00 ,  0 ,  2 , 12.0 }, 
  { " of the Guerillias"      ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  
    0 ,  0 ,  0 ,  0 ,  10 ,  10 ,  0 , 0 ,  0 , 00 , 00 , 00 , 
    00 , 00 , 00 , 00 ,  0 ,  2 , 12.0 }, 
  { " of the GPL"             ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 ,  
    0 ,  0 ,  0 ,  0 ,   0 ,   0 , 10 , 0 , 10 , 00 , 00 , 00 , 
    00 , 00 , 00 , 00 ,  0 ,  2 , 15.0 },

  { " of the LDP Project"     ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 , 
    35 , 35 ,  0 ,  0 ,   0 ,   0 ,  0 , 0 ,  0 , 00 , 00 , 00 ,
    00 , 00 , 00 , 00 ,  0 ,  2 , 16.0 },


  { "*** END OF SUFFIX LIST ***"     ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   0 ,   0 , 
    00 , 00 ,  0 ,  0 ,   0 ,   0 ,  0 , 0 ,  0 , 00 , 00 , 00 ,
    00 , 00 , 00 , 00 ,  0 ,  0 ,  1.0 }

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

spell_skill_spec SpellSkillMap[ NUMBER_OF_SKILLS ] = {
  { 0 , "Takeover/Talk/Loot"     , "TakeoverTalkSkill.png"     , UNLOADED_ISO_IMAGE , { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 } ,
  "This is the Takeover and Talk skill.  The Tux is gifted with an innert ability to hack any machine in no time.  To make use of this ability, select this skill.  When a machine is close enough and this skill is activated by holding down the right mouse button, the takeover process will be initiated." } , 
  { 0 , "Loot Chest/Body"        , "ChestAndDeadBodyLoot.png"  , UNLOADED_ISO_IMAGE , { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 } ,
  "This is the skill you must ready in order to open chests or search dead bodies.  To do this, just ready the skill and stand in front of the chest or over the dead body and press the right mouse button to open the chest/corpse looting interface." } ,
  { 0 , "Repair Skill"           , "RepairSkill.png"           , UNLOADED_ISO_IMAGE , { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 } ,
  "This skill allows to repair a given item from inventory.  To use this skill, select it and then open the inventory screen and then right click the mouse on the item you wish to repair.  While repair doesn't 'cost' anything, the item will still lose some of its maximum duration in the process, but the current duration will be restored to this new maximum value." } ,
  { 0 , "Attack with weapon"     , "WeaponSkill.png"           , UNLOADED_ISO_IMAGE , { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 } ,
  "This is the main weapon skill.  Right clicking somewhere will now cause an attack.  You can still use left click to move around.  In general, peaceful humans will not appreciate people running around with weapons drawn." } ,
  { 0 , "Identify Item"     , "IdentifySkill.png"           , UNLOADED_ISO_IMAGE , { 15 , 13 , 11 , 9 , 7 , 5 , 4 , 3 , 2 , 1 } ,
  "This skill can be used to identify items.  To use this skill, select it and then open the inventory screen and then right click the mouse on the item you wish to identify." } ,
  { 1 , "Force -> Energy "       , "FirstAidSpell.png"         , UNLOADED_ISO_IMAGE , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This is maybe the most vital spell/skill of them all.  With this spell, you can regain some of your health lost in combat for a comparatively little cost of mana." } ,
  { 1 , "Teleport Old Town" , "TeleportHomeSpell.png"     , UNLOADED_ISO_IMAGE , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This is spell allows the caster to immediately teleport to the central square of the old town.  To do this, select this skill and right click the mouse once.  It should take effect immediately." } ,

  { 1 , "Plasma Bolt"             , "PlasmaBoltSpell.png"       , UNLOADED_ISO_IMAGE , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This is a very simple combat spell.  It fires a plasma bolt into the direction the caster is facing.  This spell does not have as much effect as some ranged weapons, but the good thing about it is that it doesn't require any ammo as well, except perhaps for some mana." } ,
  { 1 , "Ice Bolt"               , "FrostBoltSpell.png"        , UNLOADED_ISO_IMAGE , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This simple combat spell allows the caster to fire a frosty bolt into the direction the caster is facing.  This frosty bold can cause some machines to seriously slow down the speed of their motions cause due to dumb MS low-level programming they think they are fighting in a winter environment and have to take extra care not to slip out." } ,
  { 1 , "Explosion Circle"       , "ExplosionCircleSpell.png"  , UNLOADED_ISO_IMAGE , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This is a very simple skill, invented more for training purposes than for actual combat situations.  It will create a circle of explosions right around the caster, but they don't do very much damage.  It's also a bit dangerous to use this spell while moving fast, cause then the caster might get himself hurt in the explosions he creates." } ,
  { 1 , "Poison Bolt"            , "PoisonBoltSpell.png"       , UNLOADED_ISO_IMAGE , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This spell creates a poisonous and acidric bolt that affects biological and mechanic entities.  The poison/acid combination will do some damage over time.  While one of these bolts is not overly effective, the effect of multiple such bots fired in rapid succession is not to be underestimated, cause damage AND duration add up, therefore doing more damage with the square of bold fired." } ,
  { 1 , "Petrification"          , "StoningBoltSpell.png"      , UNLOADED_ISO_IMAGE , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This spell creates simple crystaline bolts, that can cause a machine or biological unit to completely freeze for a certain time." } ,

  { 1 , "Detect Stuff"           , "DetectItemsSpell.png"        , UNLOADED_ISO_IMAGE , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This is spell will allow the caster to immediately detect all items not contained in any chest or dead body.  They will appear in the automatic map as yellow dots." } ,
  { 1 , "Radial EMP Wave"        , "RadialEMPWaveSpell.png"      , UNLOADED_ISO_IMAGE , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This spell creates a rapidly spreading radial electro magnetic pulse around the caster.  It can be highly effective against large numbers of weak enemies." } ,
  { 1 , "Radial VMX Wave"        , "RadialVMXWaveSpell.png"      , UNLOADED_ISO_IMAGE , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This spell creates a rapidly spreading radial wave of VMX poison gas.  While very harmful to any biological life forms it can also do some damage to organic parts build into certain droids.  This spell can also be highly effective against large numbers of weak enemies." } ,
  { 1 , "Radial Plasma Wave"     , "RadialPlasmaWaveSpell.png"   , UNLOADED_ISO_IMAGE , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This spell creates a rapidly spreading radial wave of VMX poison gas.  While very harmful to any biological life forms it can also do some damage to organic parts build into certain droids.  This spell can also be highly effective against large numbers of weak enemies." } ,
  { 1 , "Ex. Plasma Transistor"  , "ExtractPlasmaTransistor.png" , UNLOADED_ISO_IMAGE , { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 } ,
  "This skill represents the knowledge of how to extract certain parts, such as plasma transistors from dead bodies of droids.  It is a passive skill, i.e. it is not nescessary to ready this skill in order to use it.  The effect automatically takes place when a dead body of a droid is examined.  Any plasma transistors that can still be used will automatically appear in the dead bodys inventory." } ,
  
  { 1 , "Extract Superconductor" , "ExtractSuperconductor.png"      , UNLOADED_ISO_IMAGE , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This skill represents the knowledge of how to extract certain parts, such as superconducting relays from dead bodies of droids.  It is a passive skill, i.e. it is not nescessary to ready this skill in order to use it.  The effect automatically takes place when a dead body of a droid is examined.  Any superconductor that can still be used will automatically appear in the dead bodys inventory." } ,
  { 1 , "Ex. Antim. Converter"   , "ExtractAntimatterConverter.png" , UNLOADED_ISO_IMAGE , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This skill represents the knowledge of how to extract certain parts, such as Antimatter-matter converters from dead bodies of droids.  It is a passive skill, i.e. it is not nescessary to ready this skill in order to use it.  The effect automatically takes place when a dead body of a droid is examined.  Any converters that can still be used will automatically appear in the dead bodys inventory." } ,
  { 1 , "Ex. Entropy Inverter"   , "ExtractEntropyInverter.png"     , UNLOADED_ISO_IMAGE , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This skill represents the knowledge of how to extract certain parts, such as entropy inverters from dead bodies of droids.  It is a passive skill, i.e. it is not nescessary to ready this skill in order to use it.  The effect automatically takes place when a dead body of a droid is examined.  Any inverters that can still be used will automatically appear in the dead bodys inventory.This is a skill" } ,
  { 1 , "Ex. Tach. Condensator"  , "ExtractTachyonCondensator.png"  , UNLOADED_ISO_IMAGE , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This skill represents the knowledge of how to extract certain parts, such as tachyon condensators from dead bodies of droids.  It is a passive skill, i.e. it is not nescessary to ready this skill in order to use it.  The effect automatically takes place when a dead body of a droid is examined.  Any condensators that can still be used will automatically appear in the dead bodys inventory." } ,
  { 0 , "Force Explosion Ray"    , "ExplosionRaySpell.png"     , UNLOADED_ISO_IMAGE , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This is a skill manly used for training purposes.  It is very simple and also of very limited use.  But every adept usually has to learn it sooner or later in the course of his training in making use of the force." } ,


  { 1 , "Unused spell Nr. 22"    , "NoSkillAvailable.png"      , UNLOADED_ISO_IMAGE , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This is a skill" } ,
  { 1 , "Unused spell Nr. 23"    , "NoSkillAvailable.png"      , UNLOADED_ISO_IMAGE , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This is a skill" } ,
  { 1 , "Unused spell Nr. 24"    , "NoSkillAvailable.png"      , UNLOADED_ISO_IMAGE , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This is a skill" } ,

  { 1 , "Unused spell Nr. 25"    , "NoSkillAvailable.png"      , UNLOADED_ISO_IMAGE , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This is a skill" } ,
  { 1 , "Unused spell Nr. 26"    , "NoSkillAvailable.png"      , UNLOADED_ISO_IMAGE , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This is a skill" } ,
  { 1 , "Unused spell Nr. 27"    , "NoSkillAvailable.png"      , UNLOADED_ISO_IMAGE , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This is a skill" } ,
  { 1 , "Unused spell Nr. 28"    , "NoSkillAvailable.png"      , UNLOADED_ISO_IMAGE , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This is a skill" } ,
  { 1 , "Unused spell Nr. 29"    , "NoSkillAvailable.png"      , UNLOADED_ISO_IMAGE , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This is a skill" } ,

  { 1 , "Unused spell Nr. 30"    , "NoSkillAvailable.png"      , UNLOADED_ISO_IMAGE , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This is a skill" } ,
  { 1 , "Unused spell Nr. 31"    , "NoSkillAvailable.png"      , UNLOADED_ISO_IMAGE , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This is a skill" } ,
  { 1 , "Unused spell Nr. 32"    , "NoSkillAvailable.png"      , UNLOADED_ISO_IMAGE , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This is a skill" } ,
  { 1 , "Unused spell Nr. 33"    , "NoSkillAvailable.png"      , UNLOADED_ISO_IMAGE , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This is a skill" } ,
  { 1 , "Unused spell Nr. 34"    , "NoSkillAvailable.png"      , UNLOADED_ISO_IMAGE , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This is a skill" } ,

  { 1 , "Unused spell Nr. 35"    , "NoSkillAvailable.png"      , UNLOADED_ISO_IMAGE , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This is a skill" } ,
  { 1 , "Unused spell Nr. 36"    , "NoSkillAvailable.png"      , UNLOADED_ISO_IMAGE , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This is a skill" } ,
  { 1 , "Unused spell Nr. 37"    , "NoSkillAvailable.png"      , UNLOADED_ISO_IMAGE , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This is a skill" } ,
  { 1 , "Unused spell Nr. 38"    , "NoSkillAvailable.png"      , UNLOADED_ISO_IMAGE , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This is a skill" } ,
  { 1 , "Unused spell Nr. 39"    , "NoSkillAvailable.png"      , UNLOADED_ISO_IMAGE , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This is a skill" } };

Druidspec Druidmap;

Bulletspec Bulletmap;

blastspec Blastmap[ALLBLASTTYPES];

char font_switchto_red [ 2 ] = { 1 , 0 };
char font_switchto_blue [ 2 ] = { 2 , 0 };
char font_switchto_neon [ 2 ] = { 3 , 0 };

int skip_initial_menus = FALSE ;
