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
/* ----------------------------------------------------------------------
 * This file contains miscellaeous helpful functions for Freedroid.
 * ---------------------------------------------------------------------- */
/*
 * This file has been checked for remains of german comments in the code
 * I you still find some, please just kill it mercilessly.
 */
#define _misc_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"

//--------------------
// This header file is needed
#if HAVE_EXECINFO_H
#  include <execinfo.h>
#endif
#if HAVE_SIGNAL_H 
#  include <signal.h>
#endif

//--------------------
// The definition of the message structure can stay here,
// because its only needed in this module.
//
typedef struct
{
  void *NextMessage;
  int MessageCreated;
  char *MessageText;
}
message, Message;

extern SDL_Surface *zoomSurface(SDL_Surface * src, double zoomx, double zoomy, int smooth);

// UNLOADED_ISO_IMAGE { NULL , 0 , 0 , NULL , 0 , 0 , 0 }

mouse_press_button AllMousePressButtons[ MAX_MOUSE_PRESS_BUTTONS ] =
  {
    { UNLOADED_ISO_IMAGE , "THIS_DOESNT_NEED_BLITTING"                      , { 216 , 480-46 ,  46 ,  14 } , TRUE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "THIS_DOESNT_NEED_BLITTING"                      , { 307 , 480-46 ,  46 ,  14 } , TRUE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "THIS_DOESNT_NEED_BLITTING"                      , { 261 , 480-46 ,  46 ,  14 } , TRUE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "THIS_DOESNT_NEED_BLITTING"                      , { 354 , 480-46 ,  46 ,  14 } , TRUE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/cha_button_red.png"               , { 307 , 480-46 ,  46 ,  14 } , TRUE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/log_button_yellow.png"            , { 216 , 480-46 ,  46 ,  14 } , TRUE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/cha_button_yellow.png"            , { 307 , 480-46 ,  46 ,  14 } , TRUE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/inv_button_yellow.png"            , { 261 , 480-46 ,  46 ,  14 } , TRUE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/ski_button_yellow.png"            , { 354 , 480-46 ,  46 ,  14 } , TRUE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/UPButton.png"                     , { 600 ,  94 ,  40 ,  40 } , TRUE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/DOWNButton.png"                   , { 600 , 316 ,  40 ,  40 } , TRUE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/LEFTButton.png"                   , { 300 ,   5 , 100 ,  50 } , TRUE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/RIGHTButton.png"                  , { 450 ,   5 , 100 ,  50 } , TRUE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/MapExitButton.png"                , {  50 ,   5 , 100 ,  50 } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/MapUnlockDoorButton_gray.png"     , { 200 ,   5 , 100 ,  50 } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/MapUnlockDoorButton_yellow.png"   , { 200 ,   5 , 100 ,  50 } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/MapUnlockDoorButton_red.png"      , { 200 ,   5 , 100 ,  50 } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/MapGunOnOffButton_gray.png"       , { 350 ,   5 , 100 ,  50 } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/MapGunOnOffButton_yellow.png"     , { 350 ,   5 , 100 ,  50 } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/MapGunOnOffButton_red.png"        , { 350 ,   5 , 100 ,  50 } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/MapSecurityButtonMiddle.png"      , {  40 , 425 , 120 ,  50 } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/MapSecurityButtonLeft.png"        , {   5 , 425 ,  30 ,  50 } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/MapSecurityButtonRight.png"       , { 170 , 425 ,  30 ,  50 } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/MapPasswordButtonMiddle.png"      , { 310 , 425 , 120 ,  50 } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/MapSecurityButtonLeft.png"        , { 280 , 425 ,  30 ,  50 } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/MapSecurityButtonRight.png"       , { 440 , 425 ,  30 ,  50 } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/MapRequestEnergyRation_green.png" , { 200 ,  60 , 100 ,  50 } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/MapRequestEnergyRation_red.png"   , { 200 ,  60 , 100 ,  50 } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/MapReadEmail_green.png"           , { 350 ,  60 , 100 ,  50 } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/MapReadEmail_red.png"             , { 350 ,  60 , 100 ,  50 } , FALSE , FALSE } ,

    { UNLOADED_ISO_IMAGE , "mouse_buttons/MapGunTypeButton1_red.png"        , { 570 ,  64 ,  64 ,  64 } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/MapGunTypeButton2_red.png"        , { 570 , 128 ,  64 ,  64 } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/MapGunTypeButton3_red.png"        , { 570 , 192 ,  64 ,  64 } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/MapGunTypeButton4_red.png"        , { 570 , 256 ,  64 ,  64 } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/MapGunOnButton_gray.png"          , { 500 ,   5 , 100 ,  50 } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/MapGunOnButton_yellow.png"        , { 500 ,   5 , 100 ,  50 } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/MapGunOnButton_red.png"           , { 500 ,   5 , 100 ,  50 } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/MapGunTypeButton1_yellow.png"     , { 570 ,  64 ,  64 ,  64 } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/MapGunTypeButton2_yellow.png"     , { 570 , 128 ,  64 ,  64 } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/MapGunTypeButton3_yellow.png"     , { 570 , 192 ,  64 ,  64 } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/MapGunTypeButton4_yellow.png"     , { 570 , 256 ,  64 ,  64 } , FALSE , FALSE } ,

    { UNLOADED_ISO_IMAGE , "mouse_buttons/ConsoleIdentifyButton_green.png"  , {  50 ,  60 , 100 ,  50 } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/ConsoleIdentifyButton_red.png"    , {  50 ,  60 , 100 ,  50 } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/ConsoleIdentifyButton_yellow.png" , {  50 ,  60 , 100 ,  50 } , FALSE , FALSE } ,

    { UNLOADED_ISO_IMAGE , "THIS_DOESNT_NEED_BLITTING"                      , { 280 ,  44 ,  37 ,  37 } , TRUE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "THIS_DOESNT_NEED_BLITTING"                      , { 536 ,  44 ,  37 ,  37 } , TRUE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "THIS_DOESNT_NEED_BLITTING"                      , { 201 , 340 ,  47 ,  47 } , TRUE , FALSE } ,

    { UNLOADED_ISO_IMAGE , "mouse_buttons/LeftShopButton.png"               , {  22 , 447 ,  26 ,  26 } , TRUE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/RightShopButton.png"              , { 576 , 447 ,  26 ,  26 } , TRUE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/LeftShopButton.png"               , {   5 ,  16 ,  26 ,  26 } , TRUE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/RightShopButton.png"              , { 580 ,  13 ,  26 ,  26 } , TRUE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/LevelEditorObjectSelectorLeft.png", {  3 ,  25 ,  15 ,  60 } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/LevelEditorObjectSelectorRight.png", { 620 ,  26 ,  15 ,  60 } , FALSE , FALSE } ,

    { UNLOADED_ISO_IMAGE , "mouse_buttons/number_selector_ok_button.png"    , { 308 , 288 ,  48 ,  48 } , TRUE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "THIS_DOESNT_NEED_BLITTING"                      , { 148 , 244 ,  35 ,  35 } , TRUE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "THIS_DOESNT_NEED_BLITTING"                      , { 404 , 244 ,  35 ,  35 } , TRUE , FALSE } ,

    { UNLOADED_ISO_IMAGE , "mouse_buttons/buy_button.png"                   , { 199 ,  98 ,  47 ,  47 } , TRUE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/sell_button.png"                  , { 199 , 153 ,  47 ,  47 } , TRUE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/get_button.png"                   , { 193 ,  93 ,  60 ,  60 } , TRUE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/put_button.png"                   , { 192 , 147 ,  60 ,  60 } , TRUE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/repair_button.png"                , { 199 , 225 ,  47 ,  47 } , TRUE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/identify_button.png"              , { 199 , 275 ,  47 ,  47 } , TRUE , FALSE } ,

    { UNLOADED_ISO_IMAGE , "THIS_DOESNT_NEED_BLITTING"                      , { 0 + 11 , 435 , 271 , 18 } , FALSE , FALSE } ,

    { UNLOADED_ISO_IMAGE , "mouse_buttons/GoLevelNorthButton.png"           , { -50-6 , -50-8 , 25 ,  25 } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/GoLevelSouthButton.png"           , { -50-6 , -4-25 , 25 ,  25 } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/GoLevelEastButton.png"            , { -25-4 , -43 , 25 , 25 } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/GoLevelWestButton.png"            , { -75-8 , -43 ,  0 , 0 } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/ExportThisLevelButton.png"        , { -60 , 90 , 0,  0 } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/LevelEditorSaveShipButton.png"    , { -90 , 90 , 0 ,  0 } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/LevelEditorToggleWaypointButton.png" , { 00 , 150 , 0 ,  0 } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/LevelEditorToggleConnectionBlueButton.png" , { 00 , 180 , 0 ,  0 } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/LevelEditorToggleConnectionRedButton.png" , { 00 , 180 , 0 ,  0 } , FALSE , FALSE } ,

    { UNLOADED_ISO_IMAGE , "mouse_buttons/LevelEditorDeleteObstacleButton.png" , { 00 , 240 , 0 ,  0 } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/LevelEditorNextObstacleButton.png" , { 00 , 270 , 0 ,  0 } , FALSE , FALSE } ,

    { UNLOADED_ISO_IMAGE , "mouse_buttons/LevelEditorBeautifyGrassButton.png" , { 00 , 90 , 0 ,  0 } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/LevelEditorZoomInButton.png"      , { 30 , 90 , 0 ,  0 } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/LevelEditorZoomOutButton.png"     , { 30 , 90 , 0 ,  0 } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/LevelEditorRecursiveFillButton.png" , { 60 , 90 , 0 ,  0 } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/LevelEditorNewObstacleLabelButton.png" , { 90 , 90 , 0 ,  0 } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/LevelEditorNewObstacleDescriptionButton.png" , { 150 , 90 , 0 ,  0 } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/LevelEditorNewMapLabelButton.png" , { 120 , 90 , 0 ,  0 } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/LevelEditorNewItemButton.png"     , { 180 , 90 , 0 ,  0 } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/LevelEditorESCButton.png"         , { 430 , 90 , 0 ,  0 } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/LevelEditorResizeLevelButton.png" , { 460 , 90 , 0 ,  0 } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/LevelEditorKeymapButton.png"      , { -120 , 90 , 0 ,  0 } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/LevelEditorQuitButton.png"        , { -30 , 90 , 0 ,  0 } , FALSE , FALSE } ,

    { UNLOADED_ISO_IMAGE , "mouse_buttons/LevelEditorUndergroundLightButton.png"    , { -30 , 150 , 30 ,  30 } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/LevelEditorUndergroundLightOffButton.png" , { -30 , 150 , 30 ,  30 } , FALSE , FALSE } ,


    { UNLOADED_ISO_IMAGE , "mouse_buttons/LevelEditorToggleTuxButton.png"        , { 210 , 90 , 0 ,  0 } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/LevelEditorToggleTuxButtonOff.png"        , { 210 , 90 , 0 ,  0 } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/LevelEditorToggleEnemiesButton.png"    , { 240 , 90 , 0 ,  0 } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/LevelEditorToggleEnemiesButtonOff.png"    , { 240 , 90 , 0 ,  0 } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/LevelEditorToggleObstaclesButton.png"  , { 270 , 90 , 0 ,  0 } , FALSE , FALSE } , 
    { UNLOADED_ISO_IMAGE , "mouse_buttons/LevelEditorToggleObstaclesButtonOff.png"  , { 270 , 90 , 0 ,  0 } , FALSE , FALSE } , 
    { UNLOADED_ISO_IMAGE , "mouse_buttons/LevelEditorToggleTooltipsButton.png"  , { 300 , 90 , 0 ,  0 } , FALSE , FALSE } , 
    { UNLOADED_ISO_IMAGE , "mouse_buttons/LevelEditorToggleTooltipsButtonOff.png"  , { 300 , 90 , 0 ,  0 } , FALSE , FALSE } , 
    { UNLOADED_ISO_IMAGE , "mouse_buttons/LevelEditorToggleCollisionRectsButton.png"  , { 330 , 90 , 0 ,  0 } , FALSE , FALSE } , 
    { UNLOADED_ISO_IMAGE , "mouse_buttons/LevelEditorToggleCollisionRectsButtonOff.png"  , { 330 , 90 , 0 ,  0 } , FALSE , FALSE } , 
    { UNLOADED_ISO_IMAGE , "mouse_buttons/LevelEditorToggleGridButton.png"  , { 360 , 90 , 0 ,  0 } , FALSE , FALSE } , 
    { UNLOADED_ISO_IMAGE , "mouse_buttons/LevelEditorToggleGridButtonFull.png"  , { 360 , 90 , 0 ,  0 } , FALSE , FALSE } , 
    { UNLOADED_ISO_IMAGE , "mouse_buttons/LevelEditorToggleGridButtonOff.png"  , { 360 , 90 , 0 ,  0 } , FALSE , FALSE } , 

    { UNLOADED_ISO_IMAGE , "mouse_buttons/LevelEditorNextItemGroup.png"     , { 55 + 64 * 8 , 32+5*66 , 0 ,  0 } , TRUE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/LevelEditorPrevItemGroup.png"     , { 55          , 32+5*66 , 0 ,  0 } , TRUE , FALSE } ,

    { UNLOADED_ISO_IMAGE , "mouse_buttons/LevelEditorNextItemGroup.png"     , { 55 + 400    , 32+5*66 , 0 ,  0 } , TRUE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/LevelEditorPrevItemGroup.png"     , { 55 + 150    , 32+5*66 , 0 ,  0 } , TRUE , FALSE } ,

    { UNLOADED_ISO_IMAGE , "mouse_buttons/LevelEditorNextItemGroup.png"     , { 55 + 400    , 72+5*66 , 0 ,  0 } , TRUE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/LevelEditorPrevItemGroup.png"     , { 55 + 150    , 72+5*66 , 0 ,  0 } , TRUE , FALSE } ,

    { UNLOADED_ISO_IMAGE , "mouse_buttons/LevelEditorCancelItemDrop.png"    , { 55 + 80     , 32+5*66 , 0 ,  0 } , TRUE , FALSE } ,

    { UNLOADED_ISO_IMAGE , "backgrounds/SaveGameBanner.png"                 , { 0 , 0 , 200 , 50 } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "backgrounds/LoadGameBanner.png"                 , { 0 , 0 , 200 , 50 } , FALSE , FALSE } ,

    { UNLOADED_ISO_IMAGE , "THIS_DOESNT_NEED_BLITTING"                      , { WEAPON_RECT_X , WEAPON_RECT_Y , WEAPON_RECT_WIDTH , WEAPON_RECT_HEIGHT } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "THIS_DOESNT_NEED_BLITTING"                      , { DRIVE_RECT_X  , DRIVE_RECT_Y  , DRIVE_RECT_WIDTH  , DRIVE_RECT_HEIGHT } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "THIS_DOESNT_NEED_BLITTING"                      , { SHIELD_RECT_X  , SHIELD_RECT_Y  , SHIELD_RECT_WIDTH , SHIELD_RECT_HEIGHT } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "THIS_DOESNT_NEED_BLITTING"                      , { AUX1_RECT_X    , AUX1_RECT_Y    , AUX1_RECT_WIDTH , AUX1_RECT_HEIGHT } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "THIS_DOESNT_NEED_BLITTING"                      , { AUX2_RECT_X    , AUX2_RECT_Y    , AUX2_RECT_WIDTH , AUX2_RECT_HEIGHT } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "THIS_DOESNT_NEED_BLITTING"                      , { HELMET_RECT_X , HELMET_RECT_Y , HELMET_RECT_WIDTH , HELMET_RECT_HEIGHT } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "THIS_DOESNT_NEED_BLITTING"                      , { ARMOUR_RECT_X  , ARMOUR_RECT_Y  , ARMOUR_RECT_WIDTH , ARMOUR_RECT_HEIGHT } , FALSE , FALSE } ,

    { UNLOADED_ISO_IMAGE , "mouse_buttons/ScrollDialogMenuUp.png"           , { 235 , (480-20-130-20-2) , 160 ,  20 } , TRUE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/ScrollDialogMenuDown.png"         , { 235 , (480-22) , 160 ,  20 } , TRUE , FALSE } ,

    { UNLOADED_ISO_IMAGE , "mouse_buttons/AttributePlusButton.png"                   , { 0 + BUTTON_MOD_X + STR_NOW_X  , STR_Y  , 38 , 22 } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/AttributePlusButton.png"                   , { 0 + BUTTON_MOD_X + STR_NOW_X  , MAG_Y  , 38 , 22 } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/AttributePlusButton.png"                   , { 0 + BUTTON_MOD_X + STR_NOW_X  , DEX_Y  , 38 , 22 } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/AttributePlusButton.png"                   , { 0 + BUTTON_MOD_X + STR_NOW_X  , VIT_Y  , 38 , 22 } , FALSE , FALSE } ,

    //--------------------
    // These two buttons are for the scrolling text during the
    // title display, the credits menu and the level editor 
    // keyboard explanation...
    //
    { UNLOADED_ISO_IMAGE , "mouse_buttons/arrow_up_for_scroll_text.png"     , {  - 65 , 10 , 73 , 98 } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/arrow_down_for_scroll_text.png"   , {  - 65 , -10-98 , 73 , 98 } , FALSE , FALSE } ,

    { UNLOADED_ISO_IMAGE , "THIS_DOESNT_NEED_BLITTING"                      , { 607 , 99 , 26 , 26 } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "THIS_DOESNT_NEED_BLITTING"                      , { 607 , 347 , 26 , 26 } , FALSE , FALSE } ,

    //--------------------
    // Druidshow exit button...
    //
    { UNLOADED_ISO_IMAGE , "THIS_DOESNT_NEED_BLITTING"                      , { 202 , 311 , 47 , 47 } , TRUE , FALSE } ,

    //--------------------
    // These are the scrollbuttons for the chat protocal inside the
    // chat window, like when talking to a character/bot.
    //
    { UNLOADED_ISO_IMAGE , "mouse_buttons/ScrollDialogMenuUp.png"           , { 342 , 3   , 160 ,  20 } , TRUE , TRUE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/ScrollDialogMenuDown.png"         , { 342 , 272 , 160 ,  20 } , TRUE , TRUE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/ScrollDialogMenuOff.png"          , { 342 , 3   , 160 ,  20 } , TRUE , TRUE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/ScrollDialogMenuOff.png"          , { 342 , 272 , 160 ,  20 } , TRUE , TRUE } ,

    //--------------------
    // These 8 buttons are for the level editor and will not need
    // any scaling...
    //
    { UNLOADED_ISO_IMAGE , "THIS_DOESNT_NEED_BLITTING"                      , { 0   , 0 , 78 ,  14 } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "THIS_DOESNT_NEED_BLITTING"                      , { 80  , 0 , 78 ,  14 } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "THIS_DOESNT_NEED_BLITTING"                      , { 160 , 0 , 78 ,  14 } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "THIS_DOESNT_NEED_BLITTING"                      , { 240 , 0 , 78 ,  14 } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "THIS_DOESNT_NEED_BLITTING"                      , { 320 , 0 , 78 ,  14 } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "THIS_DOESNT_NEED_BLITTING"                      , { 400 , 0 , 78 ,  14 } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "THIS_DOESNT_NEED_BLITTING"                      , { 480 , 0 , 78 ,  14 } , FALSE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "THIS_DOESNT_NEED_BLITTING"                      , { 560 , 0 , 78 ,  14 } , FALSE , FALSE } ,


    { UNLOADED_ISO_IMAGE , "THIS_DOESNT_NEED_BLITTING"                      , { 486 , 324  , 73 ,  73 } , TRUE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/quest_browser_open_quests.png"    , { 473 , 97  , 153 ,  38 } , TRUE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/quest_browser_open_quests_off.png" , { 473 , 97  , 153 ,  38 } , TRUE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/quest_browser_done_quests.png"    , { 478 , 149 , 153 ,  38 } , TRUE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/quest_browser_done_quests_off.png" , { 478 , 149 , 153 ,  38 } , TRUE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/quest_browser_notes.png"          , { 478 , 203 , 153 ,  38 } , TRUE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/quest_browser_notes_off.png"      , { 478 , 203 , 153 ,  38 } , TRUE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/ScrollDialogMenuUp.png"           , { 183 , 8   , 160 ,  20 } , TRUE , TRUE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/ScrollDialogMenuDown.png"         , { 183 , 450 , 160 ,  20 } , TRUE , TRUE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/quest_browser_item_short.png"     , { 108 , 86  , 26  ,  26 } , FALSE , TRUE } ,
    { UNLOADED_ISO_IMAGE , "mouse_buttons/quest_browser_item_long.png"      , { 108 , 86  , 26  ,  26 } , FALSE , TRUE } ,

    { UNLOADED_ISO_IMAGE , "mouse_buttons/takeover_help_button.png"         , { 78 , 23 , 153 ,  38 } , FALSE , FALSE } ,

    // ------------------
    // This button is for changing the current weapon mode/reloading
    { UNLOADED_ISO_IMAGE , "THIS_DOESNT_NEED_BLITTING"			    , { CURRENT_WEAPON_RECT_X, 400, CURRENT_WEAPON_RECT_W, CURRENT_WEAPON_RECT_H } , TRUE , FALSE } ,
    { UNLOADED_ISO_IMAGE , "THIS_DOESNT_NEED_BLITTING"			    , { CURRENT_SKILL_RECT_X, 400, CURRENT_SKILL_RECT_W, CURRENT_SKILL_RECT_H } , TRUE , FALSE } ,

  }; // mouse_press_button AllMousePressButtons[ MAX_MOUSE_PRESS_BUTTONS ] 

//--------------------
// We make these global variables here, as we might want to use
// this function inside a signal handler and maybe also it's better
// not to mess too much around with the stack while trying to read
// out the stack...
//
#define MAX_CALLS_IN_BACKTRACE 2000
    void *backtrace_array [ MAX_CALLS_IN_BACKTRACE ];
    size_t backtrace_size;
    char **backtrace_strings;
    size_t backtrace_counter;

#ifndef __WIN32__
struct sigaction new_action, old_action;
#endif

/* ---------------------------------------------------------------------- 
 * Obtain a backtrace and print it to stdout.
 * ---------------------------------------------------------------------- */
void
print_trace ( int signum )
{

#if (!defined __WIN32__) && (!defined __APPLE_CC__) && (defined HAVE_BACKTRACE)

    // fprintf ( stderr , "print_trace:  Now attempting backtrace from within the code!\n" );
    // fprintf ( stderr , "print_trace:  Allowing a maximum of %d function calls on the stack!\n" , MAX_CALLS_IN_BACKTRACE );
    
    //--------------------
    // We attempt to get a backtrace of all function calls so far, even
    // including the operating system (or rather libc) call to main() in 
    // the beginning of execution.
    //
    backtrace_size = backtrace ( backtrace_array , MAX_CALLS_IN_BACKTRACE );

    fprintf ( stderr , "print_trace:  Obtained %zd stack frames.\n", backtrace_size );
    
    //--------------------
    // Now we attempt to translate the trace information we've got to the
    // symbol names that might still reside in the binary.
    //
    // NOTE: that in order for this to work, the -rdynamic switch must have
    //       been passed as on option to the LINKER!
    //       Also there might be a problem with non-ELF binaries, but let's
    //       hope that it still works...
    //
    backtrace_strings = backtrace_symbols ( backtrace_array , backtrace_size );
    
    fprintf ( stderr , "print_trace:  Obtaining symbols now done.\n" );
    
    for ( backtrace_counter = 0 ; backtrace_counter < backtrace_size ; backtrace_counter++ )
	fprintf ( stderr , "%s\n", backtrace_strings [ backtrace_counter ] );
    
    //--------------------
    // The strings generated in the backtrace_symbols function need to 
    // get freed.  Well, this isn't terribly important, but clean.
    //
    free ( backtrace_strings );

#endif

    if ( signum == SIGSEGV )
    {
	fprintf ( stderr , "\n%s():  received SIGSEGV!\n" , __FUNCTION__ );
    }
    else if ( signum == SIGFPE )
	fprintf ( stderr , "\n%s():  received SIGFPE!\n" , __FUNCTION__ );
    else if ( signum == FREEDROID_INTERNAL_ERROR_SIGNAL )
	fprintf ( stderr , "\n%s():  received INTERNAL FREEDROID ERROR SIGNAL!\n" , __FUNCTION__ );
    else
    {
    	fprintf ( stderr , "\n%s():  received UNKNOWN SIGNAL!  ERROR! \n" , __FUNCTION__ );
	Terminate ( ERR );
    }

    Terminate ( ERR );

}; // void print_trace ( int sig_num )


/* ---------------------------------------------------------------------- 
 * In this function, we move the normal SIGSEGV handler (and other signal
 * handlers to our own handler stuff, which will print out a backtrace
 * of the preceeding function calls, so that we get some suitable debug
 * output, even if there was no debugger used when starting the game.
 *
 * This migth not be completely portable to win32 systems.  Don't know if
 * it's our fault or not, but maybe we will have to disable this piece
 * of code via simple conditional compilation if the target is win32.
 *
 * For more documentation, see the GLIBC manual, Section 24.3.4 on signal
 * handling.
 *
 * ---------------------------------------------------------------------- */
void
implant_backtrace_into_signal_handlers ( void )
{

#if (!defined __WIN32__) && (!defined __APPLE_CC__)

    DebugPrintf ( -4 , "\n-Signal Handling------------------------------------------------------\n\
Setting up signal handlers for internal backtrace:\n\
Now catching SIGSEGV: " );

    //--------------------
    // We set up the structure for the new signal handling
    // to give to the opterating system
    //
    new_action . sa_handler = print_trace;
    sigemptyset ( & new_action . sa_mask );
    new_action . sa_flags = 0;

    //--------------------
    // Now it's time to activate the new signal handling...
    //
    sigaction (SIGSEGV, NULL, &old_action);
    if ( old_action . sa_handler != SIG_IGN )
    {
	DebugPrintf ( -4 , "YES" );
	sigaction ( SIGSEGV , &new_action , NULL);
    }
    else
    {
	DebugPrintf ( -4 , "NO" );
    }

    //--------------------
    // 
    DebugPrintf ( -4 , "\nNow catching FPE (if raised, that is!): " );
    sigaction (SIGFPE, NULL, &old_action);
    if ( old_action . sa_handler != SIG_IGN )
    {
	DebugPrintf ( -4 , "YES" );
	sigaction ( SIGFPE , &new_action , NULL);
    }
    else
    {
	DebugPrintf ( -4 , "NO" );
    }
    DebugPrintf ( -4 , "\n\n" );

#endif

}; // void implant_backtrace_into_signal_handlers ( void )

/* ----------------------------------------------------------------------
 * If we want the screen resolution to be a runtime option and not a 
 * compile time option any more, we must not use it as a constant.  That
 * means we must adapt the button positions to the current screeen 
 * resolution at runtime to, so we do it in this function, which will be
 * involed at program startup.
 * ---------------------------------------------------------------------- */
void
adapt_button_positions_to_screen_resolution( void )
{
    int i;

    for ( i = 0 ; i < MAX_MOUSE_PRESS_BUTTONS ; i ++ )
    {
	if ( AllMousePressButtons[ i ] . button_rect . x < 0 )
	    AllMousePressButtons[ i ] . button_rect . x += GameConfig . screen_width ;
	if ( AllMousePressButtons[ i ] . button_rect . y < 0 )
	    AllMousePressButtons[ i ] . button_rect . y += GameConfig . screen_height ;
    }

    AllMousePressButtons [ OPEN_CLOSE_SKILL_EXPLANATION_BUTTON ] . button_rect . x += CHARACTERRECT_X ;    

    AllMousePressButtons [ MORE_STR_BUTTON ] . button_rect . x += CHARACTERRECT_X ;    
    AllMousePressButtons [ MORE_MAG_BUTTON ] . button_rect . x += CHARACTERRECT_X ;    
    AllMousePressButtons [ MORE_DEX_BUTTON ] . button_rect . x += CHARACTERRECT_X ;    
    AllMousePressButtons [ MORE_VIT_BUTTON ] . button_rect . x += CHARACTERRECT_X ;    

    AllMousePressButtons [ LOAD_GAME_BANNER ] . button_rect . x = (GameConfig . screen_width-200)/2 ;
    AllMousePressButtons [ LOAD_GAME_BANNER ] . button_rect . y = (GameConfig . screen_height-50)/2 ;
    AllMousePressButtons [ SAVE_GAME_BANNER ] . button_rect . x = (GameConfig . screen_width-200)/2 ;
    AllMousePressButtons [ SAVE_GAME_BANNER ] . button_rect . y = (GameConfig . screen_height-50)/2 ;

    Droid_Image_Window . x = 48*GameConfig . screen_width/640 ;
    Droid_Image_Window . y = 44*GameConfig . screen_height/480 ;
    Droid_Image_Window . w = 130*GameConfig . screen_width/640 ;
    Droid_Image_Window . h = 172*GameConfig . screen_height/480 ;

    Full_Screen_Rect . x = 0 ;
    Full_Screen_Rect . y = 0 ; 
    Full_Screen_Rect . w = GameConfig . screen_width ;
    Full_Screen_Rect . h = GameConfig . screen_height ;

    Classic_User_Rect . x = (GameConfig . screen_width-9*INITIAL_BLOCK_WIDTH) / 2 ;
    Classic_User_Rect . y = 2*(GameConfig . screen_height-4*INITIAL_BLOCK_HEIGHT)/3 ;
    Classic_User_Rect . w = 9*INITIAL_BLOCK_WIDTH ;
    Classic_User_Rect . h = 4*INITIAL_BLOCK_HEIGHT ;

    Full_User_Rect . x = 0 ;
    Full_User_Rect . y = 0 ;
    Full_User_Rect . w = GameConfig . screen_width ;
    Full_User_Rect . h = GameConfig . screen_height ;

    Cons_Rect . x =  16 ;
    Cons_Rect . y = 162 ;
    Cons_Rect . w = 595 ;
    Cons_Rect . h = 315 ;

    Cons_Text_Rect . x = 175 ;
    Cons_Text_Rect . y = 180 ;
    Cons_Text_Rect . w = GameConfig . screen_width-175 ;
    Cons_Text_Rect . h = 305 ;


}; // void adapt_button_positions_to_screen_resolution( void )

/* ----------------------------------------------------------------------
 * This function checks if a given screen position lies within the 
 * inventory screen toggle button or not.
 * ---------------------------------------------------------------------- */
int
MouseCursorIsInRect ( SDL_Rect* our_rect , int x , int y )
{
    //--------------------
    // Now we can start to check if the mouse cursor really is on that
    // rectangle or not.
    //
    if ( x > our_rect -> x + our_rect -> w ) return ( FALSE );
    if ( x < our_rect -> x ) return ( FALSE );
    if ( y > our_rect -> y + our_rect -> h ) return ( FALSE );
    if ( y < our_rect -> y ) return ( FALSE );
    
    //--------------------
    // So since the cursor is not outside of this rectangle, it must
    // we inside, and so we'll return this answer.
    //
    return ( TRUE );
    
}; // int MouseCursorIsInRect( SDL_rect* our_rect , int x , int y )

/* ----------------------------------------------------------------------
 * This function checks if a given screen position lies within the 
 * inventory screen toggle button or not.
 * ---------------------------------------------------------------------- */
int
MouseCursorIsOnButton( int ButtonIndex , int x , int y )
{
    SDL_Rect temp_rect;

    //--------------------
    // First a sanity check if the button index given does make
    // some sense.
    //
    if ( ( ButtonIndex >= MAX_MOUSE_PRESS_BUTTONS ) || ( ButtonIndex < 0 ) )
    {
	GiveStandardErrorMessage ( __FUNCTION__  , "\
A Button that should be checked for mouse contact was requested, but the\n\
button index given exceeds the number of buttons defined in freedroid.",
				   PLEASE_INFORM, IS_FATAL );
    }

    Copy_Rect ( AllMousePressButtons [ ButtonIndex ] . button_rect , temp_rect );
    //--------------------
    // If this button needs scaling still, then we do it now...
    //
    if ( AllMousePressButtons [ ButtonIndex ] . scale_this_button )
    {
	temp_rect . x *= ((float)GameConfig . screen_width)/640.0 ;
	temp_rect . w *= ((float)GameConfig . screen_width)/640.0 ;
	temp_rect . y *= ((float)GameConfig . screen_height)/480.0 ;
	temp_rect . h *= ((float)GameConfig . screen_height)/480.0 ;
    }

    if ( y < AllMousePressButtons[ ButtonIndex ] . button_rect . y ) return ( FALSE );

    //--------------------
    // So since the cursor is not outside of this rectangle, it must
    // we inside, and so we'll return this answer.
    //
    return ( MouseCursorIsInRect ( & ( temp_rect ) , x , y ) ) ;

}; // int MouseCursorIsOnButton( int ButtonIndex , int x , int y )

/* ----------------------------------------------------------------------
 * This function blits a button to the screen.  The button must have been
 * defined prior to this in the above button list.
 * ---------------------------------------------------------------------- */
void 
UpdateScreenOverButtonFromList ( int ButtonIndex )
{ 
  our_SDL_update_rect_wrapper ( Screen , 
		   AllMousePressButtons[ ButtonIndex ] . button_rect . x ,
		   AllMousePressButtons[ ButtonIndex ] . button_rect . y ,
		   AllMousePressButtons[ ButtonIndex ] . button_rect . w ,
		   AllMousePressButtons[ ButtonIndex ] . button_rect . h 
		   );
}; // void UpdateScreenOverButtonFromList ( int ButtonIndex )

/* ----------------------------------------------------------------------
 * This function blits a button to the screen.  The button must have been
 * defined prior to this in the above button list.
 * ---------------------------------------------------------------------- */
void 
ShowGenericButtonFromList ( int ButtonIndex )
{ 
    SDL_Surface *tmp;
char fpath[2048];
    SDL_Rect Temp_Blitting_Rect;
    
    //--------------------
    // First a sanity check if the button index given does make
    // some sense.
    //
    //
    if ( ( ButtonIndex >= MAX_MOUSE_PRESS_BUTTONS ) || ( ButtonIndex < 0 ) )
    {
	GiveStandardErrorMessage ( __FUNCTION__  , 
				   "A Button that should be displayed on the screen was requested, but the\n\
button index given exceeds the number of buttons defined in freedroid.",
				   PLEASE_INFORM, IS_FATAL );
    }
    
    //--------------------
    // Now check if this button needs blitting, and if not, we do the scaling once
    // and disable the scaling ever afterwards...
    //
    if ( ! strcmp ( AllMousePressButtons[ ButtonIndex ] . button_image_file_name, 
		    "THIS_DOESNT_NEED_BLITTING") ) 
    {
	return ;
    }
    
    //--------------------
    // Now we check if we have to load the button image still
    // or if it is perhaps already loaded into memory.
    //
    if ( ( AllMousePressButtons [ ButtonIndex ] . button_image . surface == NULL ) &&
	 ( ! AllMousePressButtons [ ButtonIndex ] . button_image . texture_has_been_created ) )
    {
	find_file (AllMousePressButtons[ ButtonIndex ] . button_image_file_name , GRAPHICS_DIR, fpath, 0);
	tmp = our_IMG_load_wrapper( fpath );
	if ( tmp == NULL )
	{
	    fprintf ( stderr , "\nfpath: %s.\nButton Index: %d.\n" , fpath , ButtonIndex ) ;
	    GiveStandardErrorMessage ( __FUNCTION__  , "\
An image file for a button that should be displayed on the screen couldn't\n\
be successfully loaded into memory.\n\
This is an indication of a severe bug/installation problem of freedroid.",
				       PLEASE_INFORM, IS_WARNING_ONLY );
	return;
	}
	AllMousePressButtons[ ButtonIndex ] . button_image . surface = our_SDL_display_format_wrapperAlpha ( tmp );
	SDL_FreeSurface ( tmp );
	
	if ( AllMousePressButtons [ ButtonIndex ] . scale_this_button )
	{
	    tmp = zoomSurface( AllMousePressButtons[ ButtonIndex ] . button_image . surface , ((float)GameConfig . screen_width)/640.0 , ((float)GameConfig . screen_height)/480.0 , TRUE );
	    SDL_FreeSurface ( AllMousePressButtons[ ButtonIndex ] . button_image . surface );
	    AllMousePressButtons[ ButtonIndex ] . button_image . surface = our_SDL_display_format_wrapperAlpha ( tmp );
	    SDL_FreeSurface ( tmp );

	    AllMousePressButtons [ ButtonIndex ] . button_rect . x *= ((float)GameConfig . screen_width)/640.0 ;
	    AllMousePressButtons [ ButtonIndex ] . button_rect . w *= ((float)GameConfig . screen_width)/640.0 ;
	    AllMousePressButtons [ ButtonIndex ] . button_rect . y *= ((float)GameConfig . screen_height)/480.0 ;
	    AllMousePressButtons [ ButtonIndex ] . button_rect . h *= ((float)GameConfig . screen_height)/480.0 ;
	    AllMousePressButtons [ ButtonIndex ] . scale_this_button = FALSE ;
	}

	//--------------------
	// Maybe we had '0' entries for the height or width of this button in the list.
	// This means that we will take the real width and the real height from the image
	// and overwrite the 0 entries with this.
	//
	if ( AllMousePressButtons[ ButtonIndex ] . button_rect . w == ( 0 ) )
	{
	    AllMousePressButtons[ ButtonIndex ] . button_rect . w =
		AllMousePressButtons[ ButtonIndex ] . button_image . surface -> w ;
	}
	if ( AllMousePressButtons[ ButtonIndex ] . button_rect . h == ( 0 ) )
	{
	    AllMousePressButtons[ ButtonIndex ] . button_rect . h =
		AllMousePressButtons[ ButtonIndex ] . button_image . surface -> h ;
	}
	
	//--------------------
	// With OpenGL output method, we'll make a texture for faster and 
	// better blitting.
	//
	if ( use_open_gl )
	{
	    make_texture_out_of_surface ( & ( AllMousePressButtons [ ButtonIndex ] . button_image ) );
	}

    }

    //--------------------
    // Now that we know we have the button image loaded, we can start
    // to blit the button image to the screen.
    //
    // But in order not to damage the original rect data, we use the
    // temp value as parameter for the SDL_Blit thing..
    //
    Copy_Rect ( AllMousePressButtons[ ButtonIndex ] . button_rect , Temp_Blitting_Rect );

    if ( use_open_gl )
    {
	blit_open_gl_texture_to_screen_position ( &AllMousePressButtons [ ButtonIndex ] . button_image , Temp_Blitting_Rect . x , Temp_Blitting_Rect . y , TRUE ) ;
    }
    else
    {
	our_SDL_blit_surface_wrapper( 
	    AllMousePressButtons[ ButtonIndex ] . button_image . surface , 
	    NULL , Screen , &Temp_Blitting_Rect );
    }
    
}; // void ShowGenericButtonFromList ( int ButtonIndex )


#define SETTINGS_STRUCTURE_RAW_DATA_STRING "\nSettings Raw Data:\n"
#define END_OF_SETTINGS_DATA_STRING "\nEnd of Settings File.\n"

#define MESPOSX 0
#define MESPOSY 64
#define MESHOEHE 8
#define MESBARBREITE 320
#define MAX_MESSAGE_LEN 100
#define MESBAR_MEM MESBARBREITE*MESHOEHE+1000

void CreateMessageBar (char *MText);
void AdvanceQueue (void);

unsigned char *MessageBar;
message *Queue = NULL;
// int ThisMessageTime=0;               /* Counter fuer Message-Timing */

long oneframedelay = 0;
long tenframedelay = 0;
long onehundredframedelay = 0;
float FPSover1 = 10;
float FPSover10 = 10;
float FPSover100 = 10;
Uint32 Now_SDL_Ticks;
Uint32 One_Frame_SDL_Ticks;
Uint32 Ten_Frame_SDL_Ticks;
Uint32 Onehundred_Frame_SDL_Ticks;
int framenr = 0;

char *our_homedir = NULL;
char *our_config_dir = NULL;

/* -----------------------------------------------------------------
 * find a given filename in subdir relative to FD_DATADIR, 
 *
 * if you pass NULL as subdir, it will be ignored
 *
 * fills in the (ALLOC'd) string and returns 0 if okay, 1 on error
 *
 * ----------------------------------------------------------------- */
int
find_file (char *fname, char *subdir, char * File_Path, int silent)
{
    int i;
    FILE *fp;  // this is the file we want to find?
    
    *File_Path=0;
    if (!fname)
    {
	GiveStandardErrorMessage ( __FUNCTION__  , "\
A find_file call has been issued to generate the full path name of a\n\
certain file, but the file name given is an empty string!\n\
This is indicates a severe bug in Freedroid.",  PLEASE_INFORM, IS_FATAL );
    }
    if (!subdir)
	subdir = "";
    
    for ( i = 0 ; i < 2 ; i++ )
    {
	if (i==0)
	    sprintf ((File_Path), "..");   /* first try local subdirs */
	if (i==1)
	    sprintf ((File_Path), "%s", FD_DATADIR); /* then the DATADIR */
	
	strcat ((File_Path), "/");
	strcat ((File_Path), subdir);
	strcat ((File_Path), "/");
	
	
	strcat ((File_Path), fname);
	
	if ( (fp = fopen ((File_Path), "r")) != NULL)  /* found it? */
	{
	    fclose (fp);
	    break;
	}
	else
	{
	    if ( i == 0 )
		DebugPrintf( 1 , "\nfind_file could not succeed with LOCAL path: %s." , File_Path );
	    else
		{
                if ( ! silent )
	    	    {
		    DebugPrintf ( -4 , "The file name was: %s.\n" , fname );
		    GiveStandardErrorMessage ( __FUNCTION__  , "File not found even in data dir (2nd attempt).", 
					   NO_NEED_TO_INFORM , IS_WARNING_ONLY );
		    }
		return 1;
	        }
	}
    } // for i 
    
    // DebugPrintf( 0 , "\nfind_file determined file path: %s." , File_Path );
    
    return 0;    
}; // char * find_file ( ... )


/* ----------------------------------------------------------------------
 * This function realises the Pause-Mode: the game process is halted,
 * while the graphics and animations are not.  This mode 
 * can further be toggled from PAUSE to CHEESE, which is
 * a feature from the original program that should probably
 * allow for better screenshots.
 * ---------------------------------------------------------------------- */
void
Pause ( void )
{
    int Pause = TRUE;
    
    Activate_Conservative_Frame_Computation();
    
    Me [ 0 ] . status = PAUSE;
    AssembleCombatPicture ( DO_SCREEN_UPDATE | USE_OWN_MOUSE_CURSOR );
    append_new_game_message ( "Pausing game..." );
    while ( PPressed () )
	SDL_Delay (1); /* Wait for the pause key to be released */

    while ( Pause )
    {
	animate_tux ( 0 );
	AnimateCyclingMapTiles ();
	DisplayBanner ( );
	AssembleCombatPicture ( USE_OWN_MOUSE_CURSOR );
/*	CenteredPutStringFont ( Screen , Message_BFont , 200 , "G A M E     P A U S E D" ) ;*/
	CenteredPutStringFont ( Screen , Menu_BFont , 200 , "GAME PAUSED" ) ;
	our_SDL_flip_wrapper ( Screen );
	
	if ( CPressed ( ) )
	{
	    Me [ 0 ] . status = CHEESE;
	    DisplayBanner ( ) ;
	    AssembleCombatPicture ( DO_SCREEN_UPDATE | USE_OWN_MOUSE_CURSOR );
	    
	    while (!SpacePressed ()); /* stay CHEESE until Space pressed */
	    while ( SpacePressed() ); /* then wait for Space released */
	    
	    Me[0].status = PAUSE;       /* return to normal PAUSE */
	} 
	
	if ( SpacePressed() || PPressed() || EscapePressed() )
	{
	    Pause = FALSE;
	    while ( SpacePressed() || PPressed() );  /* wait for release, but not for escape, so that it brings the menu just after the game has come out of 
																		Pause mode */
	}
	
	//--------------------
	// During the Pause mode, there is again no need to hog the CPU and to 
	// go at full force.  We introduce some rest for the CPU here...
	//
	SDL_Delay (1);
	
    } // while (Pause) 

    append_new_game_message ( "Game resumed." );
    return;

}; // void Pause ( void ) 

/* ----------------------------------------------------------------------
 * This function starts the time-taking process.  Later the results
 * of this function will be used to calculate the current framerate
 * 
 * Two methods of time-taking are available.  One uses the SDL 
 * ticks.  This seems LESS ACCURATE.  The other one uses the
 * standard ansi c gettimeofday functions and are MORE ACCURATE
 * but less convenient to use.
 * ---------------------------------------------------------------------- */
void 
StartTakingTimeForFPSCalculation(void)
{
  /* This ensures, that 0 is never an encountered framenr,
   * therefore count to 100 here
   * Take the time now for calculating the frame rate
   * (DO NOT MOVE THIS COMMAND PLEASE!) */
  framenr++;
  
#ifdef USE_SDL_FRAMERATE
  One_Frame_SDL_Ticks=SDL_GetTicks();
  if (framenr % 10 == 1)
    Ten_Frame_SDL_Ticks=SDL_GetTicks();
  if (framenr % 100 == 1)
    {
      Onehundred_Frame_SDL_Ticks=SDL_GetTicks();
      // printf("\n%f",1/Frame_Time());
      // printf("Me[0].pos.x: %g Me[0].pos.y: %g Me[0].speed.x: %g Me[0].speed.y: %g \n",
      //Me[0].pos.x, Me[0].pos.y, Me[0].speed.x, Me[0].speed.y );
      //printf("Me[0].maxspeed.x: %g \n",
      //	     Druidmap[Me[0].type].maxspeed );
    }
#else
  gettimeofday (&oneframetimestamp, NULL);
  if (framenr % 10 == 1)
    gettimeofday (&tenframetimestamp, NULL);
  if (framenr % 100 == 1)
    {
      gettimeofday (&onehundredframetimestamp, NULL);
      printf("\n%f",1/Frame_Time());
    }
#endif
  
}; // void StartTakingTimeForFPSCalculation(void)

/* ----------------------------------------------------------------------
 * This function computes the framerate that has been experienced
 * in this frame.  It will be used to correctly calibrate all 
 * movements of game objects.
 * 
 * NOTE:  To query the actual framerate a DIFFERENT function must
 *        be used, namely Frame_Time().
 *
 *        Two methods of time-taking are available.  One uses the SDL 
 *        ticks.  This seems LESS ACCURATE.  The other one uses the
 *        standard ansi c gettimeofday functions and are MORE ACCURATE
 *        but less convenient to use.
 * ---------------------------------------------------------------------- */
void 
ComputeFPSForThisFrame(void)
{
  // static int time;
  // static int FPS_Displayed;

  // In the following paragraph the framerate calculation is done.
  // There are basically two ways to do this:
  // The first way is to use SDL_GetTicks(), a function measuring milliseconds
  // since the initialisation of the SDL.
  // The second way is to use gettimeofday, a standard ANSI C function I guess,
  // defined in time.h or so.
  // 
  // I have arranged for a definition set in defs.h to switch between the two
  // methods of ramerate calculation.  THIS MIGHT INDEED MAKE SENSE, SINCE THERE
  // ARE SOME UNEXPLAINED FRAMERATE PHENOMENA WHICH HAVE TO TO WITH KEYBOARD
  // SPACE KEY, SO PLEASE DO NOT ERASE EITHER METHOD.  PLEASE ASK JP FIRST.
  //

#ifdef USE_SDL_FRAMERATE

  Now_SDL_Ticks = SDL_GetTicks();
  oneframedelay = Now_SDL_Ticks-One_Frame_SDL_Ticks;
  tenframedelay = Now_SDL_Ticks-Ten_Frame_SDL_Ticks;
  onehundredframedelay = Now_SDL_Ticks-Onehundred_Frame_SDL_Ticks;
  
  if ( !oneframedelay ) FPSover1 = 1000 * 1 / 0.5 ;
  else FPSover1 = 1000 * 1 / (float) oneframedelay;

  if ( !tenframedelay ) FPSover10 = 1000 * 10 / 0.5 ;
  else FPSover10 = 1000 * 10 / (float) tenframedelay;

  if ( !tenframedelay ) FPSover100 = 1000 * 100 / 0.5 ;
  FPSover100 = 1000 * 100 / (float) onehundredframedelay;
  
#else
  
  gettimeofday (&now, NULL);
  
  oneframedelay =
    (now.tv_usec - oneframetimestamp.tv_usec) + (now.tv_sec -
						 oneframetimestamp.
						 tv_sec) * 1000000;
  if (framenr % 10 == 0)
    tenframedelay =
      ((now.tv_usec - tenframetimestamp.tv_usec)) + (now.tv_sec -
						     tenframetimestamp.
						     tv_sec) *
      1000000;
  if ((framenr % 100) == 0)
    {
      onehundredframedelay =
	(now.tv_sec - onehundredframetimestamp.tv_sec) * 1000000 +
	(now.tv_usec - onehundredframetimestamp.tv_usec);
      framenr = 0;
    }
  
  FPSover1 = 1000000 * 1 / (float) oneframedelay;
  FPSover10 = 1000000 * 10 / (float) tenframedelay;
  FPSover100 = 1000000 * 100 / (float) onehundredframedelay;
  
#endif

  /*
  time++;
  if ( time > 100 )
    {
      time = 0 ;
      if ( Frame_Time() > 0 )
	FPS_Displayed=(int)(1.0/Frame_Time());
      else
	FPS_Displayed=(int)9999;
      // TimeSinceLastFPSUpdate=0;
      
      DebugPrintf ( -2 , "\nFPS_Displayed: %d. " , FPS_Displayed );
    }
  */

}; // void ComputeFPSForThisFrame(void)

/* ----------------------------------------------------------------------
 *
 * This function is the key to independence of the framerate for various game elements.
 * It returns the average time needed to draw one frame.
 * Other functions use this to calculate new positions of moving objects, etc..
 *
 * Also there is of course a serious problem when some interuption occurs, like e.g.
 * the options menu is called or the debug menu is called or the console or the elevator
 * is entered or a takeover game takes place.  This might cause HUGE framerates, that could
 * box the influencer out of the ship if used to calculate the new position.
 *
 * To counter unwanted effects after such events we have the SkipAFewFramerates counter,
 * which instructs Rate_To_Be_Returned to return only the overall default framerate since
 * no better substitute exists at this moment.  But on the other hand, this seems to
 * work REALLY well this way.
 *
 * This counter is most conveniently set via the function Activate_Conservative_Frame_Computation,
 * which can be conveniently called from eveywhere.
 *
 * ---------------------------------------------------------------------- */
float
Frame_Time (void)
{
  float Rate_To_Be_Returned;
  
  if ( SkipAFewFrames ) 
    {
      Rate_To_Be_Returned = Overall_Average;
      return Rate_To_Be_Returned;
    }

  Rate_To_Be_Returned = (1.0 / FPSover1);

  return Rate_To_Be_Returned;

}; // float Frame_Time ( void )

int
Get_Average_FPS ( void )
{
  return ( (int) ( 1.0 / Overall_Average ) );
}; // int Get_Average_FPS( void )

/* ----------------------------------------------------------------------
 * 
 * With framerate computation, there is a problem when some interuption occurs, like e.g.
 * the options menu is called or the debug menu is called or the console or the elevator
 * is entered or a takeover game takes place.  This might cause HUGE framerates, that could
 * box the influencer out of the ship if used to calculate the new position.
 *
 * To counter unwanted effects after such events we have the SkipAFewFramerates counter,
 * which instructs Rate_To_Be_Returned to return only the overall default framerate since
 * no better substitute exists at this moment.
 *
 * This counter is most conveniently set via the function Activate_Conservative_Frame_Computation,
 * which can be conveniently called from eveywhere.
 *
 * ---------------------------------------------------------------------- */
void 
Activate_Conservative_Frame_Computation(void)
{
  // SkipAFewFrames=212;
  // SkipAFewFrames=22;
  SkipAFewFrames=3;

  DebugPrintf ( 1 , "\nConservative_Frame_Computation activated!" );

}; // void Activate_Conservative_Frame_Computation(void)

/* ----------------------------------------------------------------------
 * This function is used to generate an integer in range of all
 * numbers from 0 to UpperBound.
 * ---------------------------------------------------------------------- */
int
MyRandom (int UpperBound)
{
  
  if ( ! UpperBound )
	return 0;

  float tmp;
  int PureRandom;
  int dice_val;    /* the result in [0, UpperBound] */

  PureRandom = rand ();
  tmp = 1.0*PureRandom/RAND_MAX; /* random number in [0;1] */

  /* 
   * we always round OFF for the resulting int, therefore
   * we first add 0.99999 to make sure that UpperBound has
   * roughly the same probablity as the other numbers 
   */
  dice_val = (int)( tmp * (1.0 * UpperBound + 0.99999) );

  return (dice_val);

}; // int MyRandom ( int UpperBound ) 


/* ----------------------------------------------------------------------
 * This function is kills all enemy robots on the whole ship.
 * It querys the user once for safety.
 * ---------------------------------------------------------------------- */
void
Armageddon (void)
{
  char key =' ';
  int i;

  printf ("\nKill all droids on ship (y/n) ? \n");
  while ((key != 'y') && (key != 'n'))
    key = getchar_raw ();
  if (key == 'n')
    return;
  else
    for (i = 0; i < MAX_ENEMYS_ON_SHIP; i++)
      {
	AllEnemys[i].energy = -10;
	// AllEnemys[i].Status = OUT;
      }
}; // void Armageddon(void)

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
move_all_items_in_area ( Level source_level ,
			 float source_start_x , float source_start_y , 
			 float source_area_width , float source_area_height ,
			 Level target_level ,
			 float target_start_x , float target_start_y )
{
    int i;
    int j;

    for ( i = 0 ; i < MAX_ITEMS_PER_LEVEL ; i ++ )
      {
	if ( source_level -> ItemList [ i ] . type <= (-1) ) continue;
	if ( source_level -> ItemList [ i ] . pos . x < source_start_x ) continue;
	if ( source_level -> ItemList [ i ] . pos . y < source_start_y ) continue;
	if ( source_level -> ItemList [ i ] . pos . x > source_start_x + source_area_width ) continue;
	if ( source_level -> ItemList [ i ] . pos . y > source_start_y + source_area_height ) continue;

	//--------------------
	// So now we know that we must copy this item.  So we go and find a new free
	// item entry on the target level.
	//
	for ( j = 0 ; j < MAX_ITEMS_PER_LEVEL ; j ++ )
	  {
	    if ( target_level -> ItemList [ j ] . type == (-1) ) break;
	  }
	if ( j >= MAX_ITEMS_PER_LEVEL )
	  {
	    GiveStandardErrorMessage ( __FUNCTION__  , 
				       "No more free item entries in target level!" ,
				       PLEASE_INFORM, IS_FATAL );
	  }

	memcpy ( & ( target_level -> ItemList [ j ] ) , & ( source_level -> ItemList [ i ] ) , sizeof ( item ) );
	target_level -> ItemList [ j ] . pos . x = target_start_x  + source_level -> ItemList [ i ] . pos . x - source_start_x ;
	target_level -> ItemList [ j ] . pos . y = target_start_y  + source_level -> ItemList [ i ] . pos . y - source_start_y ;

	DeleteItem ( & ( source_level -> ItemList [ i ] ) );

    }
  

}; // void move_all_items_in_area ( ... )

/* ----------------------------------------------------------------------
 * It might be the case that some items come to lie on the floor inside
 * the interface area of two levels.  That might cause strange problems
 * when trying to get close in order to pick up the items.  In face we've
 * had a bug report concerning exactly this case.  So we'll fix this with
 * the following solution:  Whenever the Tux changes level, all items in
 * interface areas will be moved to the level where the Tux is.  This is
 * not compatible with multi-player game.  But multiplayer game is so far
 * away that we need not worry about this possibility right now...
 * ---------------------------------------------------------------------- */
void
move_all_items_to_level ( int target_level )
{
  int source_level_num ;
  int AreaWidth;
  int AreaHeight;

  //--------------------
  // We must check all four interface areas and maybe grab the items from
  // there...
  //

  //--------------------
  // Start to check north interface area
  //
  source_level_num = curShip . AllLevels [ target_level ] -> jump_target_north ;
  if ( source_level_num != (-1) ) 
    {
      //--------------------
      // First we find out the dimensions of the area we want to copy
      //
      if ( curShip . AllLevels [ source_level_num ] -> xlen < curShip . AllLevels [ target_level ] -> xlen )
	AreaWidth = curShip . AllLevels [ source_level_num ] -> xlen;
      else 
	AreaWidth = curShip . AllLevels [ target_level ] -> xlen ;
      
      AreaHeight = curShip . AllLevels [ target_level ] -> jump_threshold_north;
      
      if ( AreaHeight <= 0 ) return;
      
      move_all_items_in_area ( curShip . AllLevels [ source_level_num ] , 0 , curShip . AllLevels [ source_level_num ] -> ylen-AreaHeight , 
			       AreaWidth , AreaHeight ,
			       curShip . AllLevels [ target_level ] , 0 , 0 ) ;

    }

  //--------------------
  // Start to check south interface area
  //
  source_level_num = curShip . AllLevels [ target_level ] -> jump_target_south ;
  if ( source_level_num != (-1) ) 
    {
      //--------------------
      // First we find out the dimensions of the area we want to copy
      //
      if ( curShip . AllLevels [ source_level_num ] -> xlen < curShip . AllLevels [ target_level ] -> xlen )
	AreaWidth = curShip . AllLevels [ source_level_num ] -> xlen;
      else 
	AreaWidth = curShip . AllLevels [ target_level ] -> xlen ;
      
      AreaHeight = curShip . AllLevels [ target_level ] -> jump_threshold_south;
      
      if ( AreaHeight <= 0 ) return;
      
      move_all_items_in_area ( curShip . AllLevels [ source_level_num ] , 0 , 0 , 
			       AreaWidth , AreaHeight ,
			       curShip . AllLevels [ target_level ] , 0 , 
			       curShip . AllLevels [ target_level ] -> ylen-AreaHeight ) ;

    }

  //--------------------
  // Start to check east interface area
  //
  source_level_num = curShip . AllLevels [ target_level ] -> jump_target_east ;
  if ( source_level_num != (-1) ) 
    {
      //--------------------
      // First we find out the dimensions of the area we want to copy
      //
      if ( curShip . AllLevels [ source_level_num ] -> ylen < curShip . AllLevels [ target_level ] -> ylen )
	AreaHeight = curShip . AllLevels [ source_level_num ] -> ylen;
      else 
	AreaHeight = curShip . AllLevels [ target_level ] -> ylen ;
      
      AreaWidth = curShip . AllLevels [ target_level ] -> jump_threshold_east;
      
      if ( AreaWidth <= 0 ) return;
      
      move_all_items_in_area ( curShip . AllLevels [ source_level_num ] , 0 , 0 , 
			       AreaWidth , AreaHeight ,
			       curShip . AllLevels [ target_level ] ,
			       curShip . AllLevels [ target_level ] -> xlen - AreaWidth , 0 ) ;

    }

  //--------------------
  // Start to check west interface area
  //
  source_level_num = curShip . AllLevels [ target_level ] -> jump_target_west ;
  if ( source_level_num != (-1) ) 
    {
      //--------------------
      // First we find out the dimensions of the area we want to copy
      //
      if ( curShip . AllLevels [ source_level_num ] -> ylen < curShip . AllLevels [ target_level ] -> ylen )
	AreaHeight = curShip . AllLevels [ source_level_num ] -> ylen;
      else 
	AreaHeight = curShip . AllLevels [ target_level ] -> ylen ;
      
      AreaWidth = curShip . AllLevels [ target_level ] -> jump_threshold_west;
      
      if ( AreaWidth <= 0 ) return;
      
      move_all_items_in_area ( curShip . AllLevels [ source_level_num ] , 
			       curShip . AllLevels [ source_level_num ] -> xlen - AreaWidth , 0 , 
			       AreaWidth , AreaHeight ,
			       curShip . AllLevels [ target_level ] ,
			       0 , 0 ) ;

    }

}; // void move_all_items_to_level ( int levelnum )

/* ----------------------------------------------------------------------
 * This function teleports the influencer to a new position on the
 * ship.  THIS CAN BE A POSITION ON A DIFFERENT LEVEL.
 * ---------------------------------------------------------------------- */
void
Teleport ( int LNum , float X , float Y , int PlayerNum , int with_sound_and_fading )
{
    int curLevel = LNum;
    int array_num = 0;
    Level tmp;
    int i;
    char game_message_text [ 5000 ] ;

    //--------------------
    // Maybe the 'teleport' really comes from a teleportation device or
    // teleport spell or maybe even from accessing some sewer accessway.
    // In that case we'll fade out the screen a bit using the gamme ramp
    // and then later back in again.  (Note that this is a blocking function
    // call, i.e. it will take a second or so each.)
    //
    if ( with_sound_and_fading ) 
    {
	fade_out_using_gamma_ramp ();
    }
    
    if ( curLevel != Me [ PlayerNum ] . pos . z )
    {	
	//--------------------
	// In case a real level change has happend,
	// we need to do a lot of work.  Therefore we start by activating
	// the conservative frame time computation to avoid a 'jump'.
	//
	Activate_Conservative_Frame_Computation();

        //--------------------
        // If the level we're coming out from is the one where the homespot is, the teleport anchor has to be removed,
        // because it means that the player decided not to teleport back to the previous location
	location HomeSpot;
	ResolveMapLabelOnShip ( "TeleportHomeTarget" , &(HomeSpot) );
        if ( Me [ PlayerNum ] . pos . z == HomeSpot . level)
		{
	        Me [ 0 ] . teleport_anchor.x = 0;
                Me [ 0 ] . teleport_anchor.y = 0;
		}


	//--------------------
	// I think this is for the unlikely case of misordered levels in 
	// the ship file used for this game?!
	//
	while ((tmp = curShip.AllLevels[array_num]) != NULL)
	{
	    if (tmp->levelnum == curLevel)
		break;
	    else
		array_num++;
	}
	
	//--------------------
	// We set a new CurLevel.  This is old and depreciated code,
	// that should sooner or later be completely deactivated.
	//
	CurLevel = curShip . AllLevels [ array_num ] ;
	
	Me [ PlayerNum ] . pos . x = X;
	Me [ PlayerNum ] . pos . y = Y;
	Me [ PlayerNum ] . pos . z = array_num; 
	
	silently_unhold_all_items ();
	move_all_items_to_level ( Me [ 0 ] . pos . z );
	silently_unhold_all_items ();
	
	//--------------------
	// We add some sanity check against teleporting to non-allowed
	// locations (like outside of map that is)
	//
	if ( ( LNum < 0 ) || ( Me [ PlayerNum ] . pos . x < 0 ) || ( Me [ PlayerNum ] . pos . y < 0 ) ||
	     ( LNum >= curShip.num_levels ) || 
	     ( Me [ PlayerNum ] . pos . x >= curShip.AllLevels[ array_num ] -> xlen ) ||
	     ( Me [ PlayerNum ] . pos . y >= curShip.AllLevels[ array_num ] -> ylen ) )
	{
	    fprintf( stderr, "\n\ntarget location was: lev=%d x=%f y=%f.\n" , LNum , X , Y );
	    fprintf( stderr, "source location was: lev=%d x=%f y=%f." , Me [ PlayerNum ] . pos . z , 
		     Me [ PlayerNum ] . pos . x , Me [ PlayerNum ] . pos . y );
	    GiveStandardErrorMessage ( __FUNCTION__  , "\
A Teleport was requested, but the location to teleport to lies outside\n\
the bounds of this 'ship' which means the current collection of levels.\n\
This indicates an error in the map system of Freedroid.",
				       PLEASE_INFORM, IS_FATAL );
	}
	
	//--------------------
	// Turn off all blasts and bullets from the old level
	//
	for (i = 0; i < MAXBLASTS; i++)
	{
	    AllBlasts[i].type = INFOUT;
	}
	for (i = 0; i < MAXBULLETS; i++)
	{
	    //--------------------
	    // Don't ever delete bullets any other way!!! SEGFAULTS might result!!!
	    // in this case, we need no bullet-explosions
	    //
	    DeleteBullet ( i , FALSE ); 
	}
	
	//--------------------
	// Since we've moved to a new level, we might also say so, a message like
	// "Entering ThisAndThat..." should appear in bold font on the screen.
	// although only if a level name is set
	//
	// This will be disabled now, as the current level name is written down in
	// the top screen text line anyway, so no need for annoying messages in 
	// the middle of the screen here.
	//
	/*
	if ( strcmp ( curShip . AllLevels [ Me [ 0 ] . pos . z ] -> Levelname, "" ) ) 
	{
	    strcpy ( entering_message , "Entering " );
	    strcat ( entering_message , curShip . AllLevels [ Me [ 0 ] . pos . z ] -> Levelname );
	    SetNewBigScreenMessage ( entering_message );
	}
	*/

	//--------------------
	// After the level has been changed, the automap texture needs to be cleared.
	// However, if the Tux has been to (parts of this) level before, we should also
	// restore the parts of the automap, that are already known to the Tux.
	//
	clear_automap_texture_completely (  );
	insert_old_map_info_into_texture (  );
	
    }
    else
    {
	//--------------------
	// If no real level change has occured, everything
	// is simple and we just need to set the new coordinates, haha
	//
	Me [ PlayerNum ] . pos . x = X ;
	Me [ PlayerNum ] . pos . y = Y ;
    }
    
    //--------------------
    // After the teleport, the mouse move target might be
    // completely out of date.  Therefore we simply delete it.  In cases
    // where the jump came from crossing a jump threshold (levels glued
    // together) we can still restore the move target in that (the calling!)
    // function.
    //
    Me [ PlayerNum ] . mouse_move_target . x = ( -1 ) ;
    Me [ PlayerNum ] . mouse_move_target . y = ( -1 ) ;
    Me [ PlayerNum ] . mouse_move_target . z = ( -1 ) ;
    
    if ( with_sound_and_fading ) 
    {
	teleport_arrival_sound ();
    }
    
    //--------------------
    // Perhaps the player is visiting this level for the first time.  Then, the
    // tux should make it's initial statement about the location, if there is one.
    //
    if ( ! Me [ PlayerNum ] . HaveBeenToLevel [ CurLevel->levelnum ] )
    {
	PlayLevelCommentSound ( CurLevel->levelnum );
	Me [ PlayerNum ] . HaveBeenToLevel [ CurLevel->levelnum ] = TRUE;
	// if ( array_num != 0 ) ShuffleEnemys ( array_num );
	// if ( ( LNum != 0 ) && ( Shuffling ) ) ShuffleEnemys ( array_num );
	// ShuffleEnemys ( array_num );
    }
    
    //--------------------
    // No more shuffling once the game is up and running...
    // else there are hostile bots inside some buildings and such things...
    //
    // if ( Shuffling ) ShuffleEnemys ( array_num );
    
    SwitchBackgroundMusicTo( CurLevel->Background_Song_Name );
    
    //--------------------
    // Since we've mightily changed position now, we should clear the
    // position history, so that noone get's confused...
    //
    InitInfluPositionHistory ( PlayerNum );
    
    
    if ( with_sound_and_fading ) 
    {
	sprintf ( game_message_text , "Arrived at %s." , curShip . AllLevels [ Me [ PlayerNum ] . pos . z ] -> Levelname );
	append_new_game_message ( game_message_text );
	AssembleCombatPicture ( SHOW_ITEMS | USE_OWN_MOUSE_CURSOR ); 
	//our_SDL_flip_wrapper ( Screen );
	StoreMenuBackground(0);
	fade_in_using_gamma_ramp ();
    }

}; // void Teleport( ... ) 

/* ----------------------------------------------------------------------
 * I'd like to use the function strnlen in LoadSettings, but this would
 * be a gnu extension. so I have to write this function myselt in order
 * to keep the code completely portable.
 * ---------------------------------------------------------------------- */
int
mystrnlen ( char* MyString , int MyMaxlen )
{
  int i;

  for ( i = 0 ; i < MyMaxlen ; i ++ )
    {
      if ( MyString [ i ] == 0 ) return i;
    }
  return MyMaxlen;
}; // int mystrnlen ( char* MyString , int MyMaxlen )

/*----------------------------------------------------------------------
 * LoadGameConfig(): load saved options from config-file
 *
 * this should be the first of all load/save functions called
 * as here we read the $HOME-dir and create the config-subdir if neccessary
 *
 *----------------------------------------------------------------------*/
int
LoadGameConfig (void)
{
    char fname[5000];
    FILE *config;
    int original_width_of_screen = GameConfig . screen_width ;
    int original_height_of_screen = GameConfig . screen_height ;
    
    if (! our_config_dir)
    {
	DebugPrintf (1, "No useable config-dir. No config-loading possible\n");
	return (OK);
    }
    
    sprintf (fname, "%s/config", our_config_dir);
    if( (config = fopen (fname, "rb")) == NULL)
    {
	DebugPrintf (0, "WARNING: failed to open config-file: %s\n");
	return (ERR);
    }
    
    //--------------------
    // Now read the actual data
    // ok, this is neither very portable nor very flexible, we just want that working...

    /* NOTE: we don't have to worry about endian-ness for the config-file
     * because this is usually written and read on the same machine ! 
     * Therefore simply freading/fwriting it should be fine
     */
    fread ( &(GameConfig), sizeof (configuration_for_freedroid), sizeof(char), config);
    fclose (config);
    
    //--------------------
    // Now we do some extra security check:  Maybe the old settings file
    // was generated by a different version of freedroid.  Then extra care
    // must be taken:  The misinterpretation of values could case segmentation
    // faults and things like that.
    //
    // For this purpose I'd like to use the function 'strnlen' but this would be 
    // a gnu extension and not standard C, so I have to use the 'mystrlen' function
    // taken from the gnu c manual.
    //
    if ( ( mystrnlen ( GameConfig.freedroid_version_string , 100 ) != strlen ( VERSION ) ) ||
	 ( memcmp ( GameConfig.freedroid_version_string , VERSION , strlen ( VERSION ) ) ) )
    {
	GiveStandardErrorMessage ( __FUNCTION__  , "\
Settings file found in your ~/.freedroid_rpg dir does not\n\
seem to be from the same version a this installation of freedroid.\n\
This is perfectly normal if you have just upgraded your version of\n\
freedroid.  But the loading of your settings will be cancelled now,\n\
cause the format of the settings file is no longer supported.  \n\
No need to panic.  The default settings will be used instead and a new\n\
settings file will be generated.",
				   NO_NEED_TO_INFORM, IS_WARNING_ONLY );
	ResetGameConfigToDefaultValues (  );
	return (ERR);
    };
    
    //--------------------
    // We may print out that config was loaded successfully...
    // This might stay in here.
    //
    DebugPrintf ( 0 , "\nSuccessfully loaded and enforced your personal configuration file %s.\n\n" , fname );
    
    //--------------------
    // Now we will turn off the skills and inventory screen and that, cause
    // this should be off when the game starts...
    //
    GameConfig . CharacterScreen_Visible = FALSE ;
    GameConfig . Inventory_Visible = FALSE ;
    GameConfig . Mission_Log_Visible = FALSE ;
    GameConfig . SkillScreen_Visible = FALSE ;
    GameConfig . skill_explanation_screen_visible = FALSE ;
    GameConfig . Automap_Visible = FALSE ;
    
    if ( command_line_override_for_screen_resolution )
    {
	GameConfig . screen_width = original_width_of_screen ;
	GameConfig . screen_height = original_height_of_screen ;
    }

    //--------------------
    // If the user is using SDL for the graphics output, then no other
    // screen resolutions than 640x480 will be available.
    //
    if ( ( ! use_open_gl ) && ( GameConfig . screen_width != 640 ) )
    {
	GameConfig . screen_width = 640; 
	GameConfig . screen_height = 480 ;
	GameConfig . next_time_width_of_screen = 640; 
	GameConfig . next_time_height_of_screen = 480 ;
	GiveStandardErrorMessage ( __FUNCTION__  , "\
You are using SDL instead of OpenGL for graphics output.  For this\n\
output method, no other screen resolutions than 640x480 is available.\n\
Therefore your setting will be overridden and 640x480 will be used.\n\
If you want different resolutions, please use OpenGL for graphics\n\
output.",
				   NO_NEED_TO_INFORM , IS_WARNING_ONLY );
    }

    //--------------------
    // In OpenGL, there is no need for the reduced menu display (which is
    // based on restoring a saved "screenshot", which isn't working so well
    // in OpenGL anyway) so we don't even allow for reduced menus in this
    // mode.
    //
    if ( use_open_gl ) GameConfig . menu_mode = MENU_MODE_DEFAULT;

    return (OK);
    
}; // int LoadGameConfig ( void )
    
/*----------------------------------------------------------------------
 * SaveGameConfig: do just that
 *
 *----------------------------------------------------------------------*/
int
SaveGameConfig (void)
{
    char fname[5000];
    FILE *config;
    int current_width;
    int current_height;

    //--------------------
    // Maybe the Terminate function was invoked BEFORE the startup process
    // was complete at all (like e.g. some illegal command line parameter).
    // Then the config dir is not initialized.  We catch this case and return
    // control to the operating system immediately if that happens...
    //
    if ( our_config_dir == NULL )
    {
	printf("It seems that the game couldn't start up at all... therefore we need not save any config information.\n\n");
	SDL_Quit();
#if __WIN32__
/*ExitCode undeclared says mingw32	if ( ExitCode == ERR )
	{*/
	    system ( "notepad stderr.txt" );
	    system ( "notepad stdout.txt" );
	/*}*/
#endif
	exit ( ERR );
    }

    //--------------------
    // Now we know, that the config dir has been initialized already.
    // That indicates, that the game did start up already.
    // Therefore we can do the normal save config stuff...
    //
    if ( our_config_dir [ 0 ] == '\0' )
	return (ERR);
    
    sprintf (fname, "%s/config", our_config_dir );
    if( (config = fopen (fname, "wb")) == NULL)
    {
	DebugPrintf (0, "WARNING: failed to create config-file: %s\n");
	return (ERR);
    }
    
    //--------------------
    // We put the current version number of freedroid into the 
    // version number string.  This will be usefull so that later
    // versions of freedroid can identify old config files and decide
    // not to use them in some cases.
    //
    strcpy ( GameConfig.freedroid_version_string , VERSION );
    
    //--------------------
    // We preseve the current resolution, modify it a bit, such that
    // the preseleted resolution will come to effect next time, save
    // it and then we restore the current settings again.
    //
    current_width = GameConfig . screen_width ;
    current_height = GameConfig . screen_height ;
    GameConfig . screen_width = GameConfig . next_time_width_of_screen ;
    GameConfig . screen_height = GameConfig . next_time_height_of_screen ;
    //--------------------
    // Now write the actual data
    fwrite ( &(GameConfig), sizeof (configuration_for_freedroid), sizeof(char), config);
    GameConfig . screen_width = current_width ;
    GameConfig . screen_height = current_height ;

    fclose (config);
    
    return (OK);
    
}; // int SaveGameConfig ( void )


/* ----------------------------------------------------------------------
 * This function is used for terminating freedroid.  It will close
 * the SDL submodules and exit.
 * ---------------------------------------------------------------------- */
void
Terminate (int ExitCode)
{
    DebugPrintf (2, "\nvoid Terminate(int ExitStatus) was called....");
    printf("\n----------------------------------------------------------------------");
    printf("\nTermination of freedroidRPG initiated...");
    
    //--------------------
    // We save the config file in any case.
    //
    SaveGameConfig();

    //--------------------
    // If this was a game server, it should close all the
    // connections to players.
    //
    if ( ServerMode )
    {
	DebugPrintf ( 0 , "\n\
--------------------\n\
Closing all players connections to this server...\n\
--------------------\n");
	DisconnectAllRemoteClinets ( ) ;
    }

    // free the mixer channels...
    // Mix_CloseAudio();

    // printf("\nAnd now the final step...\n\n");
    printf("Thank you for playing freedroidRPG.\n\n");
    SDL_Quit();
    
    //--------------------
    // Occasionally we want the debugger to stop here.  That can be
    // done by simply forcing a segmentation fault violation signal here,
    // even if there hasn't been any real segmentation fault.
    //
    // raise ( SIGSEGV );

    //--------------------
    // Finally, especially on win32 systems, we should open an editor with
    // the last debug output, since people in general won't know how and where
    // to find the material for proper reporting of bugs.
    //
#if __WIN32__
    if ( ExitCode == ERR )
    {
	system ( "notepad stderr.txt" );
	system ( "notepad stdout.txt" );
    }
#endif

    //--------------------
    // Now we drop control back to the operating system.  The FreedroidRPG
    // program has finished.
    //
    exit ( ExitCode );

    return;
}; // void Terminate ( int ExitCode )

/* ----------------------------------------------------------------------
 * Since numbers are not so very telling and can easily get confusing
 * we do not use numbers to reference the action from a trigger but 
 * rather we use labels already in the mission file.  However internally
 * the game needs numbers as a pointer or index in a list and therefore
 * this functions was added to go from a label to the corresponding 
 * number entry.
 * ---------------------------------------------------------------------- */
int
GiveNumberToThisActionLabel ( char* ActionLabel )
{
  int i;

  // DebugPrintf( 1 , "\nvoid ExecuteEvent ( int EventNumber ) : real function call confirmed. ");
  // DebugPrintf( 1 , "\nvoid ExecuteEvent ( int EventNumber ) : executing event labeld : %s" , ActionLabel );

  // In case of 'none' as action label, we don't do anything and return;
  if ( strcmp ( ActionLabel , "none" ) == 0 ) return ( -1 );

  //--------------------
  // Now we find out which index the desired action has
  //
  for ( i = 0 ; i < MAX_TRIGGERED_ACTIONS_IN_GAME ; i++ )
    {
      if ( strcmp ( AllTriggeredActions[ i ].ActionLabel , ActionLabel ) == 0 ) break;
    }

  if ( i >= MAX_TRIGGERED_ACTIONS_IN_GAME )
    {
      fprintf( stderr, "\n\nActionLabel: '%s'\n" , ActionLabel );
      GiveStandardErrorMessage ( __FUNCTION__  , "\
The label that should reference an action for later execution could not\n\
be identified as valid reference to an existing action.",
				 PLEASE_INFORM, IS_FATAL );
    }

  return ( i );
}; // int GiveNumberToThisActionLabel ( char* ActionLabel )

/* ----------------------------------------------------------------------
 * This function executes an action with a label.
 * ---------------------------------------------------------------------- */
void 
ExecuteActionWithLabel ( char* ActionLabel , int PlayerNum )
{
    ExecuteEvent( GiveNumberToThisActionLabel ( ActionLabel ) , PlayerNum );
}; // void ExecuteActionWithLabel ( char* ActionLabel )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
obstacle* 
give_pointer_to_obstacle_with_label ( char* obstacle_label ) 
{
  int i, j , k;

  //--------------------
  // Now we try to resolve the obstacle given in the labels of all the decks
  // of the current ship.
  //
  for ( i = 0 ; i < curShip . num_levels ; i ++ )
    {
      for ( j = 0 ; j < MAX_OBSTACLE_NAMES_PER_LEVEL ; j ++ )
	{
	  if ( curShip . AllLevels [ i ] -> obstacle_name_list [ j ] != NULL )
	    {
	      if ( strcmp ( curShip . AllLevels [ i ] -> obstacle_name_list [ j ] , obstacle_label ) == 0 )
		{
		  //--------------------
		  // Now we need to find out which obstacle is pointing to this label
		  //
		  for ( k = 0 ; k < MAX_OBSTACLES_ON_MAP ; k ++ )
		    {
		      if ( curShip . AllLevels [ i ] -> obstacle_list [ k ] . name_index == j )
			{
			  DebugPrintf ( 0 , "\nSUCCESSFULLY RESOLVED OBSTACLE NAME %s." , 
					obstacle_label );
			  return ( & ( curShip . AllLevels [ i ] -> obstacle_list [ k ] ) );
			}
		    }
		  //--------------------
		  // So here we've encountered an error!  There seems to be no obstacle
		  // pointing to this obstacle label (any more).
		  //
		  fprintf ( stderr , "\nlabel=\"%s\"" , obstacle_label );
		  GiveStandardErrorMessage ( __FUNCTION__  , "\
The obstacle label was found in the label list,\n but no obstacle seems to point to this label.\nA possible reason for this could be a duplicate entry in the label list.\nDevelopers will have to fix manually..." ,
					     PLEASE_INFORM, IS_FATAL );
		}
	    }
	}
    }

  //--------------------
  // So here we've encountered an error!  There seems to be no obstacle
  // pointing to this obstacle label (any more).
  //
  GiveStandardErrorMessage ( __FUNCTION__  , "\
The obstacle label given was NOT found in any levels obstacle label list." ,
			     PLEASE_INFORM, IS_FATAL );
  return ( NULL ) ;

}; // obstacle* give_pointer_to_obstacle_with_label ( char* obstacle_label ) 

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
int
give_level_of_obstacle_with_label ( char* obstacle_label ) 
{
  int i, j , k;

  //--------------------
  // Now we try to resolve the obstacle given in the labels of all the decks
  // of the current ship.
  //
  for ( i = 0 ; i < curShip . num_levels ; i ++ )
    {
      for ( j = 0 ; j < MAX_OBSTACLE_NAMES_PER_LEVEL ; j ++ )
	{
	  if ( curShip . AllLevels [ i ] -> obstacle_name_list [ j ] != NULL )
	    {
	      if ( strcmp ( curShip . AllLevels [ i ] -> obstacle_name_list [ j ] , obstacle_label ) == 0 )
		{
		  //--------------------
		  // Now we need to find out which obstacle is pointing to this label
		  //
		  for ( k = 0 ; k < MAX_OBSTACLES_ON_MAP ; k ++ )
		    {
		      if ( curShip . AllLevels [ i ] -> obstacle_list [ k ] . name_index == j )
			{
			  DebugPrintf ( 0 , "\nSUCCESSFULLY RESOLVED OBSTACLE NAME %s." , 
					obstacle_label );
			  return ( i ) ;
			}
		    }
		  //--------------------
		  // So here we've encountered an error!  There seems to be no obstacle
		  // pointing to this obstacle label (any more).
		  //
		  GiveStandardErrorMessage ( __FUNCTION__  , "\
The obstacle label was found in the label list,\n but no obstacle seems to point to this label." ,
					     PLEASE_INFORM, IS_FATAL );
		}
	    }
	}
    }

  //--------------------
  // So here we've encountered an error!  There seems to be no obstacle
  // pointing to this obstacle label (any more).
  //
  GiveStandardErrorMessage ( __FUNCTION__  , "\
The obstacle label given was NOT found in any levels obstacle label list." ,
			     PLEASE_INFORM, IS_FATAL );
  return ( -1 ) ;

}; // int give_level_of_obstacle_with_label ( char* obstacle_label ) 

/* ----------------------------------------------------------------------
 * 
 * ---------------------------------------------------------------------- */
void 
ExecuteEvent ( int EventNumber , int PlayerNum )
{
    obstacle* our_obstacle;
    int obstacle_level_num ;
    Level obstacle_level ;
    
    DebugPrintf( 1 , "\nvoid ExecuteEvent ( int EventNumber ) : executing event Nr.: %d." , EventNumber );

    // Do nothing in case of the empty action (-1) given.
    if ( EventNumber == (-1) ) return;
    
    // Does the action include a change of a map tile?
    if ( AllTriggeredActions [ EventNumber ] . ChangeMapTo != -1 )
    {
	//YES.  So we need to check, if the location has been supplied fully first.
	if ( ( AllTriggeredActions[ EventNumber ].ChangeMapLocation.x == (-1) ) ||
	     ( AllTriggeredActions[ EventNumber ].ChangeMapLocation.y == (-1) ) ||
	     ( AllTriggeredActions[ EventNumber ].ChangeMapLevel == (-1) ) )
	{
	    DebugPrintf( 0 , "\n\nSorry! There has been a corrupt event specification!\n\nTerminating...\n\n");
	    Terminate(ERR);
	}
	else
	{
	    DebugPrintf( 1 , "\nvoid ExecuteEvent ( int EventNumber ) : Change map Event correctly specified. confirmed.");
	    curShip.AllLevels[ AllTriggeredActions[ EventNumber ].ChangeMapLevel ]->map [ AllTriggeredActions[ EventNumber ].ChangeMapLocation.y ] [ AllTriggeredActions[ EventNumber ].ChangeMapLocation.x ]  . floor_value  = AllTriggeredActions[ EventNumber ].ChangeMapTo ;
	    
	    GetAllAnimatedMapTiles ( curShip.AllLevels[ AllTriggeredActions[ EventNumber ].ChangeMapLevel ]  );
	}
    }
    
    //--------------------
    // Maybe the action will cause a map obstacle to change type.  If that is so, we
    // do it here...
    //
    if ( strlen ( AllTriggeredActions [ EventNumber ] . modify_obstacle_with_label ) > 0 )
    {
	our_obstacle = give_pointer_to_obstacle_with_label ( AllTriggeredActions [ EventNumber ] . modify_obstacle_with_label ) ;
	obstacle_level_num = give_level_of_obstacle_with_label ( AllTriggeredActions [ EventNumber ] . modify_obstacle_with_label ) ;
	obstacle_level = curShip . AllLevels [ obstacle_level_num ] ;
	if ( AllTriggeredActions [ EventNumber ] . modify_obstacle_to_type )
		our_obstacle -> type = AllTriggeredActions [ EventNumber ] . modify_obstacle_to_type ;
	else {
		delete_obstacle (  curShip . AllLevels [ obstacle_level_num ], our_obstacle);
	     }

	//--------------------
	// Now we make sure the door lists and that are all updated...
	//
	
	GetAllAnimatedMapTiles ( curShip . AllLevels [ obstacle_level_num ] ) ;
	//--------------------
	// Also make sure the other maps realize the change too, if it
	// maybe happend in the border area where two maps are glued together
	// only export if the obstacle falls within the interface zone
	
	if( our_obstacle->pos.x <= obstacle_level->jump_threshold_west ||
	    our_obstacle->pos.x >= obstacle_level->xlen - obstacle_level->jump_threshold_east ||
	    our_obstacle->pos.y <= obstacle_level->jump_threshold_north ||
	    our_obstacle->pos.y >= obstacle_level->ylen - obstacle_level->jump_threshold_south
	    ) ExportLevelInterface ( obstacle_level_num ) ;
	
    }

    // Does the action include a teleport of the influencer to some other location?
    if ( AllTriggeredActions [ EventNumber ] . TeleportTarget . x != (-1) )
    {
	DebugPrintf( 1 , "\nvoid ExecuteEvent: Now a teleportation should occur!" );
	Teleport ( AllTriggeredActions[ EventNumber ].TeleportTargetLevel ,
		   AllTriggeredActions[ EventNumber ].TeleportTarget.x + 0.5 ,
		   AllTriggeredActions[ EventNumber ].TeleportTarget.y + 0.5 ,
		   PlayerNum , TRUE );
    }
    
    // Does the defined action assign the influencer a mission?
    if ( AllTriggeredActions[ EventNumber ].AssignWhichMission != (-1) )
    {
	AssignMission( AllTriggeredActions[ EventNumber ].AssignWhichMission );
    }
    
    // Does the defined action make the influencer say something?
    if ( strlen ( AllTriggeredActions[ EventNumber ].InfluencerSayText ) > 0 )
    {
	//YES. So we need to output his sentence as usual
	Me[0].TextVisibleTime=0;
	Me[0].TextToBeDisplayed=AllTriggeredActions[ EventNumber ].InfluencerSayText;
    }

    // Does the defined action change another action trigger?
    if ( strlen ( AllTriggeredActions[ EventNumber ] . modify_event_trigger_with_action_label ) > 0 )
	{
	//look for the target event trigger
	event_trigger * target_event = NULL;
	int i;
        for ( i = 0 ; i < MAX_EVENT_TRIGGERS ; i++ )
		{
		if ( ! strcmp (AllEventTriggers[i].TargetActionLabel ,  AllTriggeredActions[ EventNumber ] . modify_event_trigger_with_action_label ) )
			target_event = &AllEventTriggers[i];
		}
	
	//Shall we disable the event trigger?
	if ( AllTriggeredActions[ EventNumber ] . modify_event_trigger_value == 0 )
		{
		target_event -> enabled = 0;
		}

	//Shall we enable the event trigger?
	if ( AllTriggeredActions[ EventNumber ] . modify_event_trigger_value == 1 )
		{
		target_event -> enabled = 1;
		}
	}

   // Does the defined action run another action ?
   if ( strlen ( AllTriggeredActions [ EventNumber ] . also_execute_action_label ) > 0 )
	{
	ExecuteActionWithLabel ( AllTriggeredActions [ EventNumber ] . also_execute_action_label, 0 );
	}
}; // void ExecuteEvent ( int EventNumber )

/* ----------------------------------------------------------------------
 *
 * This function checks for triggered events & statements.  Those events are
 * usually entered via the mission file and read into the apropriate
 * structures via the InitNewMission function.  Here we check, whether
 * the nescessary conditions for an event are satisfied, and in case that
 * they are, we order the apropriate event to be executed.
 *
 * In addition, statements are started, if the influencer is at the 
 * right location for them.
 *
 * ---------------------------------------------------------------------- */
void 
CheckForTriggeredEventsAndStatements ( int PlayerNum )
{
    int i;
    int map_x, map_y;
    Level StatementLevel = curShip.AllLevels[ Me [ PlayerNum ] . pos . z ] ;
    
    //--------------------
    // Now we check if some statment location is reached
    //
    map_x = (int) rintf( (float) Me [ PlayerNum ] . pos . x ); map_y = (int) rintf( (float)Me [ PlayerNum ] . pos . y ) ;
    for ( i = 0 ; i < MAX_STATEMENTS_PER_LEVEL ; i++ )
    {
	if ( ( map_x == StatementLevel -> StatementList [ i ] . x ) &&
	     ( map_y == StatementLevel -> StatementList [ i ] . y ) )
	{
	    Me [ PlayerNum ] . TextVisibleTime = 0 ;
	    Me [ PlayerNum ] . TextToBeDisplayed = CurLevel -> StatementList [ i ] . Statement_Text ;
	}
    }
    
    //--------------------
    // Now we check if some event trigger is fullfilled.
    //
    for ( i = 0 ; i < MAX_EVENT_TRIGGERS ; i++ )
    {
	// if ( AllEventTriggers[i].EventNumber == (-1) ) continue;  // thats a sure sign this event doesn't need attention
	if ( strcmp (AllEventTriggers[i].TargetActionLabel , "none" ) == 0 ) continue;  // thats a sure sign this event doesn't need attention
	if ( AllEventTriggers[i].enabled == 0 ) continue;  // this trigger is not enabled
	
	// --------------------
	// So at this point we know, that the event trigger is somehow meaningful. 
	// Fine, so lets check the details, if the event is triggered now
	//
	
	if ( AllEventTriggers[i].Influ_Must_Be_At_Point.x != (-1) )
	{
	    if ( rintf( AllEventTriggers[i].Influ_Must_Be_At_Point.x ) != (int) ( Me [ PlayerNum ] . pos.x ) ) continue;
	}
	
	if ( AllEventTriggers[i].Influ_Must_Be_At_Point.y != (-1) )
	{
	    if ( rintf( AllEventTriggers[i].Influ_Must_Be_At_Point.y ) != (int) ( Me [ PlayerNum ] . pos.y ) ) continue;
	}
	
	if ( AllEventTriggers[i].Influ_Must_Be_At_Level != (-1) )
	{
	    if ( rintf( AllEventTriggers[i].Influ_Must_Be_At_Level ) != StatementLevel->levelnum ) continue;
	}
	
	DebugPrintf( 2 , "\nWARNING!! INFLU NOW IS AT SOME TRIGGER POINT OF SOME LOCATION-TRIGGERED EVENT!!!");
	// ExecuteEvent( AllEventTriggers[i].EventNumber );
	ExecuteActionWithLabel ( AllEventTriggers [ i ] . TargetActionLabel , PlayerNum ) ;
	
	if ( AllEventTriggers[i].DeleteTriggerAfterExecution == 1 )
	{
	    // AllEventTriggers[i].EventNumber = (-1); // That should prevent the event from being executed again.
	    AllEventTriggers[i].TargetActionLabel = "none"; // That should prevent the event from being executed again.
	}
    }

}; // CheckForTriggeredEventsAndStatements (void )


/*----------------------------------------------------------------------
 * Copyright (C) 1997-2001 Id Software, Inc., under GPL
 *
 * va(fmt, ...)
 * 
 * does a varargs printf into a temp buffer, so I don't need to have
 * varargs versions of all text functions.
 *----------------------------------------------------------------------*/
char
*va (char *format, ...)
{
  va_list	argptr;
  static char	string[1024];
  
  va_start (argptr, format);
  vsprintf (string, format,argptr);
  va_end (argptr);
  
  return string;	
}

/*----------------------------------------------------------------------
 * try getting round endian-differences with minimal intervention
 * to the code.. 
 *
 * read out a 2-byte short-int from give memory pointer, either using
 * the given byte-order (PCs) or SDL's 'network byte order' (Mac)
 *
 *
 * FIXME: Ideally all binar read/write should use SDLNet_Read/Write fcts, 
 * but this would imply larger changes to the code...
 *----------------------------------------------------------------------*/
Sint16 
ReadSint16 (void * memory)
{
  Sint16 ret;

  memcpy (&ret, memory, sizeof(Sint16));
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  endian_swap ( (char*)&ret, sizeof (Sint16), 1);
#endif

  return (ret);

} /* ReadSint16() */

/***********************************************************************
 * a little endian-swapper 
 */
void 
endian_swap(char * pdata, size_t dsize, size_t nelements)
{
  unsigned int i, j, indx;
  char tempbyte;

  if (dsize <= 1) return;

  for (i=0; i<nelements; i++)
    {
      indx = dsize;
      for (j=0; j<dsize/2; j++)
	{
	  tempbyte = pdata[j];
	  indx = indx - 1;
	  pdata[j] = pdata[indx];
	  pdata[indx] = tempbyte;
	}
      
      pdata = pdata + dsize;
    }
  
  return;

} /* endian swap */



#undef _misc_c
