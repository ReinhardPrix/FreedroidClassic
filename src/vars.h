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

SDL_Rect Cons_Menu_Rect = {32, 180, CONS_MENU_LENGTH, CONS_MENU_HEIGHT};
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

obstacle_spec obstacle_map [ NUMBER_OF_OBSTACLE_TYPES ] = 
  {
    // iso_image                block_area_type     bl_parm1 bl_parm2 smashable preput drop_treas. filename
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0000.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0001.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0002.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0003.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0004.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0005.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0006.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0007.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0008.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0009.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0010.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0011.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0012.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0013.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0014.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0015.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0016.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0017.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0018.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0019.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0020.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0021.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0022.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0023.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0024.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0025.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0026.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0027.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0028.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0029.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0030.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0031.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0032.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0033.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0034.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0035.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0036.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0037.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0038.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0039.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0040.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0041.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0042.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0043.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0044.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0045.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0046.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0047.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0048.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0049.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0050.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0051.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0052.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0053.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0054.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0055.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0056.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0057.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0058.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0059.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0060.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0061.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0062.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0063.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0064.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0065.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0066.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0067.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0068.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0069.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0070.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0071.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0072.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0073.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0074.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0075.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0076.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0077.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0078.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0079.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0080.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0081.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0082.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0083.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0084.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0085.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0086.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0087.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0088.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0089.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0090.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0091.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0092.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0093.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0094.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0095.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0096.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0097.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0098.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0099.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0100.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0101.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0102.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0103.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0104.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0105.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0106.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0107.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0108.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0109.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0110.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0111.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0112.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0113.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0114.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0115.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0116.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0117.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0118.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0119.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0120.png" },
    { { NULL , 0 , 0 , NULL } , COLLISION_TYPE_RECTANGLE , 1.2 , 0.25 , FALSE , FALSE , FALSE , "iso_obstacle_0121.png" }
  };


item_image_spec ItemImageList[ NUMBER_OF_ITEM_PICTURES ] = {
  { { 1 , 1 } , NULL } ,  { { 2 , 2 } , NULL } ,  { { 2 , 2 } , NULL } ,  { { 2 , 2 } , NULL } , // 0-3
  { { 2 , 2 } , NULL } ,  { { 2 , 2 } , NULL } ,  { { 2 , 2 } , NULL } ,  { { 1 , 1 } , NULL } , // 4
  { { 2 , 2 } , NULL } ,  { { 2 , 2 } , NULL } ,  { { 2 , 2 } , NULL } ,  { { 2 , 2 } , NULL } , // 8
  { { 2 , 2 } , NULL } ,  { { 2 , 2 } , NULL } ,  { { 2 , 2 } , NULL } ,  { { 2 , 2 } , NULL } , // 12
  { { 2 , 2 } , NULL } ,  { { 1 , 1 } , NULL } ,  { { 2 , 2 } , NULL } ,  { { 2 , 2 } , NULL } , // 16
  { { 2 , 2 } , NULL } ,  { { 2 , 2 } , NULL } ,  { { 1 , 3 } , NULL } ,  { { 2 , 3 } , NULL } , // 20
  { { 2 , 2 } , NULL } ,  { { 2 , 3 } , NULL } ,  { { 2 , 3 } , NULL } ,  { { 1 , 1 } , NULL } , // 24
  { { 1 , 2 } , NULL } ,  { { 1 , 1 } , NULL } ,  { { 1 , 1 } , NULL } ,  { { 2 , 3 } , NULL } , // 28
  { { 2 , 2 } , NULL } ,  { { 1 , 1 } , NULL } ,  { { 1 , 1 } , NULL } ,  { { 1 , 1 } , NULL } , // 32
  { { 2 , 2 } , NULL } ,  { { 2 , 2 } , NULL } ,  { { 2 , 2 } , NULL } ,  { { 2 , 2 } , NULL } , // 36
  { { 2 , 3 } , NULL } ,  { { 1 , 1 } , NULL } ,  { { 1 , 1 } , NULL } ,  { { 2 , 2 } , NULL } , // 40
  { { 2 , 2 } , NULL } ,  { { 1 , 1 } , NULL } ,  { { 2 , 2 } , NULL } ,  { { 2 , 2 } , NULL } , // 44
  { { 1 , 1 } , NULL } ,  { { 1 , 1 } , NULL } ,  { { 1 , 1 } , NULL } ,  { { 1 , 1 } , NULL } , // 48-51
  { { 1 , 1 } , NULL } ,  { { 1 , 1 } , NULL } ,  { { 1 , 1 } , NULL } ,  { { 1 , 1 } , NULL } , // 52-55
  { { 1 , 1 } , NULL } ,  { { 1 , 1 } , NULL } ,  { { 2 , 2 } , NULL } ,  { { 1 , 1 } , NULL } , // 56-59
  { { 1 , 1 } , NULL } ,  { { 1 , 1 } , NULL } ,  { { 1 , 1 } , NULL } ,  { { 1 , 1 } , NULL } , // 60-63
  { { 1 , 1 } , NULL } ,  { { 1 , 1 } , NULL } ,  { { 1 , 1 } , NULL } ,  { { 1 , 1 } , NULL }   // 64-67
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
  { 0 , "Takeover/Talk/Loot"     , "TakeoverTalkSkill.png"     , NULL , { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 } ,
  "This is the Takeover and Talk skill.  The Tux is gifted with an innert ability to hack any machine in no time.  To make use of this ability, select this skill.  When a machine is close enough and this skill is activated by holding down the right mouse button, the takeover process will be initiated." } ,
  { 0 , "Loot Chest/Body"        , "ChestAndDeadBodyLoot.png"  , NULL , { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 } ,
  "This is the skill you must ready in order to open chests or search dead bodies.  To do this, just ready the skill and stand in front of the chest or over the dead body and press the right mouse button to open the chest/corpse looting interface." } ,
  { 0 , "Repair Skill"           , "RepairSkill.png"           , NULL , { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 } ,
  "This skill allows to repair a given item from inventory.  To use this skill, select it and then open the inventory screen and then right click the mouse on the item you wish to repair.  While repair doesn't 'cost' anything, the item will still lose some of its maximum duration in the process, but the current duration will be restored to this new maximum value." } ,
  { 0 , "Attack with weapon"     , "WeaponSkill.png"           , NULL , { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 } ,
  "This is the main weapon skill.  Right clicking somewhere will now cause an attack.  You can still use left click to move around.  In general, peaceful humans will not appreciate people running around with weapons drawn." } ,
  { 1 , "Force -> Energy "       , "FirstAidSpell.png"         , NULL , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This is maybe the most vital spell/skill of them all.  With this spell, you can regain some of your health lost in combat for a comparatively little cost of mana." } ,
  { 1 , "Teleport Old Town" , "TeleportHomeSpell.png"     , NULL , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This is spell allows the caster to immediately teleport to the central square of the old town.  To do this, select this skill and right click the mouse once.  It should take effect immediately." } ,

  { 1 , "Plasma Bolt"             , "PlasmaBoltSpell.png"       , NULL , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This is a very simple combat spell.  It fires a plasma bolt into the direction the caster is facing.  This spell does not have as much effect as some ranged weapons, but the good thing about it is that it doesn't require any ammo as well, except perhaps for some mana." } ,
  { 1 , "Ice Bolt"               , "FrostBoltSpell.png"        , NULL , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This simple combat spell allows the caster to fire a frosty bolt into the direction the caster is facing.  This frosty bold can cause some machines to seriously slow down the speed of their motions cause due to dumb MS low-level programming they think they are fighting in a winter environment and have to take extra care not to slip out." } ,
  { 1 , "Explosion Circle"       , "ExplosionCircleSpell.png"  , NULL , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This is a very simple skill, invented more for training purposes than for actual combat situations.  It will create a circle of explosions right around the caster, but they don't do very much damage.  It's also a bit dangerous to use this spell while moving fast, cause then the caster might get himself hurt in the explosions he creates." } ,
  { 1 , "Poison Bolt"            , "PoisonBoltSpell.png"       , NULL , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This spell creates a poisonous and acidric bolt that affects biological and mechanic entities.  The poison/acid combination will do some damage over time.  While one of these bolts is not overly effective, the effect of multiple such bots fired in rapid succession is not to be underestimated, cause damage AND duration add up, therefore doing more damage with the square of bold fired." } ,
  { 1 , "Petrification"          , "StoningBoltSpell.png"      , NULL , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This spell creates simple crystaline bolts, that can cause a machine or biological unit to completely freeze for a certain time." } ,

  { 1 , "Detect Stuff"           , "DetectItemsSpell.png"        , NULL , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This is spell will allow the caster to immediately detect all items not contained in any chest or dead body.  They will appear in the automatic map as yellow dots." } ,
  { 1 , "Radial EMP Wave"        , "RadialEMPWaveSpell.png"      , NULL , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This spell creates a rapidly spreading radial electro magnetic pulse around the caster.  It can be highly effective against large numbers of weak enemies." } ,
  { 1 , "Radial VMX Wave"        , "RadialVMXWaveSpell.png"      , NULL , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This spell creates a rapidly spreading radial wave of VMX poison gas.  While very harmful to any biological life forms it can also do some damage to organic parts build into certain droids.  This spell can also be highly effective against large numbers of weak enemies." } ,
  { 1 , "Radial Plasma Wave"     , "RadialPlasmaWaveSpell.png"   , NULL , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This spell creates a rapidly spreading radial wave of VMX poison gas.  While very harmful to any biological life forms it can also do some damage to organic parts build into certain droids.  This spell can also be highly effective against large numbers of weak enemies." } ,
  { 1 , "Ex. Plasma Transistor"  , "ExtractPlasmaTransistor.png" , NULL , { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 } ,
  "This skill represents the knowledge of how to extract certain parts, such as plasma transistors from dead bodies of droids.  It is a passive skill, i.e. it is not nescessary to ready this skill in order to use it.  The effect automatically takes place when a dead body of a droid is examined.  Any plasma transistors that can still be used will automatically appear in the dead bodys inventory." } ,
  
  { 1 , "Extract Superconductor" , "ExtractSuperconductor.png"      , NULL , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This skill represents the knowledge of how to extract certain parts, such as superconducting relays from dead bodies of droids.  It is a passive skill, i.e. it is not nescessary to ready this skill in order to use it.  The effect automatically takes place when a dead body of a droid is examined.  Any superconductor that can still be used will automatically appear in the dead bodys inventory." } ,
  { 1 , "Ex. Antim. Converter"   , "ExtractAntimatterConverter.png" , NULL , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This skill represents the knowledge of how to extract certain parts, such as Antimatter-matter converters from dead bodies of droids.  It is a passive skill, i.e. it is not nescessary to ready this skill in order to use it.  The effect automatically takes place when a dead body of a droid is examined.  Any converters that can still be used will automatically appear in the dead bodys inventory." } ,
  { 1 , "Ex. Entropy Inverter"   , "ExtractEntropyInverter.png"     , NULL , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This skill represents the knowledge of how to extract certain parts, such as entropy inverters from dead bodies of droids.  It is a passive skill, i.e. it is not nescessary to ready this skill in order to use it.  The effect automatically takes place when a dead body of a droid is examined.  Any inverters that can still be used will automatically appear in the dead bodys inventory.This is a skill" } ,
  { 1 , "Ex. Tach. Condensator"  , "ExtractTachyonCondensator.png"  , NULL , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This skill represents the knowledge of how to extract certain parts, such as tachyon condensators from dead bodies of droids.  It is a passive skill, i.e. it is not nescessary to ready this skill in order to use it.  The effect automatically takes place when a dead body of a droid is examined.  Any condensators that can still be used will automatically appear in the dead bodys inventory." } ,
  { 0 , "Force Explosion Ray"    , "ExplosionRaySpell.png"     , NULL , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This is a skill manly used for training purposes.  It is very simple and also of very limited use.  But every adept usually has to learn it sooner or later in the course of his training in making use of the force." } ,


  { 1 , "Unused spell Nr. 21"    , "NoSkillAvailable.png"      , NULL , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This is a skill" } ,
  { 1 , "Unused spell Nr. 22"    , "NoSkillAvailable.png"      , NULL , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This is a skill" } ,
  { 1 , "Unused spell Nr. 23"    , "NoSkillAvailable.png"      , NULL , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This is a skill" } ,
  { 1 , "Unused spell Nr. 24"    , "NoSkillAvailable.png"      , NULL , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This is a skill" } ,

  { 1 , "Unused spell Nr. 25"    , "NoSkillAvailable.png"      , NULL , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This is a skill" } ,
  { 1 , "Unused spell Nr. 26"    , "NoSkillAvailable.png"      , NULL , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This is a skill" } ,
  { 1 , "Unused spell Nr. 27"    , "NoSkillAvailable.png"      , NULL , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This is a skill" } ,
  { 1 , "Unused spell Nr. 28"    , "NoSkillAvailable.png"      , NULL , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This is a skill" } ,
  { 1 , "Unused spell Nr. 29"    , "NoSkillAvailable.png"      , NULL , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This is a skill" } ,

  { 1 , "Unused spell Nr. 30"    , "NoSkillAvailable.png"      , NULL , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This is a skill" } ,
  { 1 , "Unused spell Nr. 31"    , "NoSkillAvailable.png"      , NULL , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This is a skill" } ,
  { 1 , "Unused spell Nr. 32"    , "NoSkillAvailable.png"      , NULL , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This is a skill" } ,
  { 1 , "Unused spell Nr. 33"    , "NoSkillAvailable.png"      , NULL , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This is a skill" } ,
  { 1 , "Unused spell Nr. 34"    , "NoSkillAvailable.png"      , NULL , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This is a skill" } ,

  { 1 , "Unused spell Nr. 35"    , "NoSkillAvailable.png"      , NULL , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This is a skill" } ,
  { 1 , "Unused spell Nr. 36"    , "NoSkillAvailable.png"      , NULL , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This is a skill" } ,
  { 1 , "Unused spell Nr. 37"    , "NoSkillAvailable.png"      , NULL , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This is a skill" } ,
  { 1 , "Unused spell Nr. 38"    , "NoSkillAvailable.png"      , NULL , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This is a skill" } ,
  { 1 , "Unused spell Nr. 39"    , "NoSkillAvailable.png"      , NULL , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This is a skill" } ,

  { 1 , "Unused spell Nr. 40"    , "NoSkillAvailable.png"      , NULL , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This is a skill" } ,
  { 1 , "Unused spell Nr. 41"    , "NoSkillAvailable.png"      , NULL , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This is a skill" } ,
  { 1 , "Unused spell Nr. 42"    , "NoSkillAvailable.png"      , NULL , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This is a skill" } ,
  { 1 , "Unused spell Nr. 43"    , "NoSkillAvailable.png"      , NULL , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This is a skill" } ,
  { 1 , "Unused spell Nr. 44"    , "NoSkillAvailable.png"      , NULL , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This is a skill" } ,

  { 1 , "Unused spell Nr. 45"    , "NoSkillAvailable.png"      , NULL , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This is a skill" } ,
  { 1 , "Unused spell Nr. 46"    , "NoSkillAvailable.png"      , NULL , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This is a skill" } ,
  { 1 , "Unused spell Nr. 47"    , "NoSkillAvailable.png"      , NULL , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This is a skill" } ,
  { 1 , "Unused spell Nr. 48"    , "NoSkillAvailable.png"      , NULL , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This is a skill" } ,
  { 1 , "Unused spell Nr. 49"    , "NoSkillAvailable.png"      , NULL , { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
  "This is a skill" } 

}; // SpellSkillSpec SpellSkillMap[ NUMBER_OF_SKILLS ] = {

Druidspec Druidmap;

Bulletspec Bulletmap;

blastspec Blastmap[ALLBLASTTYPES];



